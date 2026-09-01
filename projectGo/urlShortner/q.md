# URL Shortener System Design

An enterprise-grade, Bit.ly-style URL shortening service — architecture, data model, algorithms, and object-oriented design following SOLID principles.

---

## 1. Requirements

### 1.1 Functional Requirements

| #   | Requirement                                | Notes                                             |
| --- | ------------------------------------------ | ------------------------------------------------- |
| F1  | Shorten a long URL into a unique short URL | Auto-generated short code                         |
| F2  | Redirect short URL → original URL          | HTTP 301/302                                      |
| F3  | Custom alias                               | User-supplied short code, must be unique          |
| F4  | Analytics                                  | Click count, timestamp, geo, user-agent, referrer |
| F5  | Expiration                                 | TTL per URL; expired links return 410/404         |
| F6  | Scale to billions of URLs                  | Horizontally scalable storage & compute           |

### 1.2 Non-Functional Requirements

| #   | Requirement             | Target                                                                                            |
| --- | ----------------------- | ------------------------------------------------------------------------------------------------- |
| N1  | Low latency             | p99 < 50ms for redirect, < 100ms for shorten                                                      |
| N2  | High availability       | 99.99% uptime, no single point of failure                                                         |
| N3  | Fault tolerance         | Failover across AZs/regions                                                                       |
| N4  | Consistency             | Strong consistency on write (no duplicate aliases); eventual consistency acceptable for analytics |
| N5  | Read-heavy optimization | Read:Write ratio ~100:1, cache-first                                                              |

### 1.3 Back-of-the-envelope Estimation

- **Scale**: 1B new URLs/month → ~400 writes/sec average, ~4,000/sec peak
- **Reads**: 100:1 ratio → ~40,000 redirects/sec average, ~400,000/sec peak
- **Storage**: 1B URLs/month × 5 years × ~500 bytes/record ≈ **30 TB** (plus analytics, which is far larger and stored separately)
- **Short code space**: base62 (`[a-zA-Z0-9]`), 7 characters → 62^7 ≈ **3.5 trillion** combinations — sufficient for decades of growth

---

## 2. High-Level Architecture

```
                              ┌───────────────────┐
                              │   Client / SDK      │
                              └─────────┬──────────┘
                                        │ HTTPS
                              ┌─────────▼──────────┐
                              │  CDN / Edge Cache    │  (cache hot redirects at edge)
                              └─────────┬──────────┘
                                        │
                              ┌─────────▼──────────┐
                              │  API Gateway / LB    │  (TLS, auth, rate limiting)
                              └─────────┬──────────┘
                      ┌─────────────────┼─────────────────┐
                      │                 │                 │
             ┌────────▼───────┐ ┌───────▼────────┐ ┌──────▼───────┐
             │ Shorten Service │ │ Redirect Service│ │ Analytics API │
             │  (stateless)    │ │  (stateless)    │ │  (stateless)  │
             └────────┬───────┘ └───────┬────────┘ └──────┬───────┘
                      │                 │                 │
             ┌────────▼───────┐ ┌───────▼────────┐        │
             │ ID Generator    │ │  Cache (Redis)  │        │
             │ (Snowflake /    │ │  cluster        │        │
             │  Range-based)   │ └───────┬────────┘        │
             └────────┬───────┘         │                 │
                      │        ┌────────▼────────┐         │
                      │        │  Metadata Store  │         │
                      └───────►│  (sharded KV/DB)  │◄────────┘
                               └────────┬────────┘
                                        │  async events (Kafka)
                               ┌────────▼────────┐
                               │ Stream Processor │
                               │ (click enrichment)│
                               └────────┬────────┘
                               ┌────────▼────────┐
                               │ Analytics Store   │
                               │ (columnar / OLAP) │
                               └───────────────────┘
```

**Key architectural decisions:**

- **Shorten** and **Redirect** are split into separate stateless services so each can be scaled and cached independently — redirect traffic is ~100x shorten traffic.
- **Redirect path never blocks on analytics.** A click event is fired asynchronously to a Kafka topic; the redirect (302) returns immediately. This decouples N1 (latency) from N4 (analytics consistency).
- **Cache-aside** pattern with Redis in front of the metadata store absorbs the read-heavy load; only cache misses hit the database.
- **ID generation is decoupled from storage writes** to avoid a single auto-increment bottleneck at billion-row scale (see §4).

---

## 3. Object-Oriented Design (SOLID)

The design favors composition over inheritance, small interfaces, and dependency inversion so that storage engines, ID strategies, and notification channels can be swapped without touching business logic. Examples below are in Python-flavored pseudocode; the same structure applies directly in Java/C#/TypeScript.

### 3.1 Core Interfaces (Dependency Inversion)

```python
from abc import ABC, abstractmethod
from dataclasses import dataclass
from datetime import datetime
from typing import Optional


# ---------- Domain Model ----------

@dataclass(frozen=True)
class ShortUrl:
    short_code: str
    long_url: str
    owner_id: Optional[str]
    created_at: datetime
    expires_at: Optional[datetime]
    is_custom_alias: bool

    def is_expired(self, now: datetime) -> bool:
        return self.expires_at is not None and now >= self.expires_at


@dataclass(frozen=True)
class ClickEvent:
    short_code: str
    timestamp: datetime
    ip_address: str
    user_agent: str
    referrer: Optional[str]
    geo_country: Optional[str] = None
    geo_city: Optional[str] = None


# ---------- Abstractions (ports) ----------

class ShortCodeGenerator(ABC):
    """Strategy interface — Open/Closed: new algorithms plug in without
    modifying the service that uses them."""

    @abstractmethod
    def generate(self, long_url: str) -> str: ...


class UrlRepository(ABC):
    """Persistence port — Dependency Inversion: the domain layer depends
    on this abstraction, not on a concrete database driver."""

    @abstractmethod
    def save(self, url: ShortUrl) -> None: ...

    @abstractmethod
    def find_by_code(self, short_code: str) -> Optional[ShortUrl]: ...

    @abstractmethod
    def exists(self, short_code: str) -> bool: ...

    @abstractmethod
    def delete_expired(self, before: datetime) -> int: ...


class CacheStore(ABC):
    @abstractmethod
    def get(self, key: str) -> Optional[str]: ...

    @abstractmethod
    def set(self, key: str, value: str, ttl_seconds: int) -> None: ...

    @abstractmethod
    def invalidate(self, key: str) -> None: ...


class ClickEventPublisher(ABC):
    """Analytics is fire-and-forget from the redirect path's perspective."""

    @abstractmethod
    def publish(self, event: ClickEvent) -> None: ...


class UrlValidator(ABC):
    @abstractmethod
    def validate(self, long_url: str) -> None:
        """Raises InvalidUrlError if invalid."""


class AliasPolicy(ABC):
    """Single Responsibility: alias rules (length, charset, reserved
    words, profanity) live here, separate from persistence logic."""

    @abstractmethod
    def validate_alias(self, alias: str) -> None: ...
```

### 3.2 Concrete Strategies (Open/Closed Principle)

New encoding strategies can be added without modifying `UrlShortenerService`.

```python
import hashlib
import base64

BASE62_ALPHABET = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"


class CounterBasedGenerator(ShortCodeGenerator):
    """Converts a globally unique integer ID (from a distributed ID
    generator) into a base62 string. Guarantees uniqueness by
    construction — no collision checking required."""

    def __init__(self, id_generator: "DistributedIdGenerator"):
        self._id_generator = id_generator

    def generate(self, long_url: str) -> str:
        unique_id = self._id_generator.next_id()
        return self._encode_base62(unique_id)

    @staticmethod
    def _encode_base62(number: int) -> str:
        if number == 0:
            return BASE62_ALPHABET[0]
        digits = []
        while number:
            number, rem = divmod(number, 62)
            digits.append(BASE62_ALPHABET[rem])
        return "".join(reversed(digits)).rjust(7, "0")


class HashBasedGenerator(ShortCodeGenerator):
    """Deterministic MD5-based generator with collision retry.
    Useful when idempotent short codes for the same long URL are
    desired (same input -> same output on first attempt)."""

    def __init__(self, repository: "UrlRepository", code_length: int = 7):
        self._repository = repository
        self._code_length = code_length

    def generate(self, long_url: str) -> str:
        salt = ""
        for attempt in range(5):
            digest = hashlib.md5((long_url + salt).encode()).digest()
            code = base64.urlsafe_b64encode(digest).decode()[: self._code_length]
            if not self._repository.exists(code):
                return code
            salt = f"-{attempt}"
        raise RuntimeError("Failed to generate unique short code after retries")
```

### 3.3 Distributed ID Generation

```python
class DistributedIdGenerator(ABC):
    @abstractmethod
    def next_id(self) -> int: ...


class SnowflakeIdGenerator(DistributedIdGenerator):
    """Twitter Snowflake-style: <timestamp><machine_id><sequence>.
    Each shorten-service node can mint IDs independently — no
    coordination or shared counter needed, which removes the single
    point of contention at billion-row scale."""

    EPOCH = 1_700_000_000_000  # custom epoch, ms

    def __init__(self, machine_id: int):
        self._machine_id = machine_id & 0x3FF  # 10 bits
        self._sequence = 0
        self._last_timestamp = -1

    def next_id(self) -> int:
        timestamp = self._current_millis()
        if timestamp == self._last_timestamp:
            self._sequence = (self._sequence + 1) & 0xFFF  # 12 bits
            if self._sequence == 0:
                timestamp = self._wait_next_millis(timestamp)
        else:
            self._sequence = 0
        self._last_timestamp = timestamp

        return (
            ((timestamp - self.EPOCH) << 22)
            | (self._machine_id << 12)
            | self._sequence
        )

    def _current_millis(self) -> int: ...
    def _wait_next_millis(self, ts: int) -> int: ...


class RangeHandoutIdGenerator(DistributedIdGenerator):
    """Alternative: a central 'Ticket Server' hands out ID ranges
    (e.g. [1,1000), [1000,2000)) to each service instance, which then
    increments locally in memory. Cheaper than Snowflake to reason
    about; trades a small blast radius on ticket-server failure."""

    def __init__(self, range_provider: "IdRangeProvider"):
        self._provider = range_provider
        self._current = 0
        self._max = 0

    def next_id(self) -> int:
        if self._current >= self._max:
            self._current, self._max = self._provider.acquire_range()
        value = self._current
        self._current += 1
        return value
```

### 3.4 Application Services (Single Responsibility + Interface Segregation)

```python
class InvalidUrlError(Exception): ...
class AliasUnavailableError(Exception): ...
class UrlExpiredError(Exception): ...
class UrlNotFoundError(Exception): ...


class UrlShortenerService:
    """Orchestrates URL creation. Depends only on abstractions
    (constructor injection) — trivially testable and swappable."""

    def __init__(
        self,
        generator: ShortCodeGenerator,
        repository: UrlRepository,
        cache: CacheStore,
        validator: UrlValidator,
        alias_policy: AliasPolicy,
        clock: "Clock",
        default_ttl_days: int = 365,
    ):
        self._generator = generator
        self._repository = repository
        self._cache = cache
        self._validator = validator
        self._alias_policy = alias_policy
        self._clock = clock
        self._default_ttl_days = default_ttl_days

    def shorten(
        self,
        long_url: str,
        custom_alias: Optional[str] = None,
        owner_id: Optional[str] = None,
        ttl_days: Optional[int] = None,
    ) -> ShortUrl:
        self._validator.validate(long_url)

        if custom_alias:
            self._alias_policy.validate_alias(custom_alias)
            if self._repository.exists(custom_alias):
                raise AliasUnavailableError(custom_alias)
            code = custom_alias
        else:
            code = self._generator.generate(long_url)

        now = self._clock.now()
        expires = now + timedelta(days=ttl_days or self._default_ttl_days)
        entity = ShortUrl(
            short_code=code,
            long_url=long_url,
            owner_id=owner_id,
            created_at=now,
            expires_at=expires,
            is_custom_alias=bool(custom_alias),
        )
        self._repository.save(entity)
        self._cache.set(code, long_url, ttl_seconds=86400)
        return entity


class RedirectService:
    """Single responsibility: resolve a short code as fast as possible.
    Cache-aside: check cache, fall back to repository, repopulate cache."""

    def __init__(
        self,
        repository: UrlRepository,
        cache: CacheStore,
        clock: "Clock",
        click_publisher: ClickEventPublisher,
    ):
        self._repository = repository
        self._cache = cache
        self._clock = clock
        self._click_publisher = click_publisher

    def resolve(self, short_code: str, request_ctx: "RequestContext") -> str:
        long_url = self._cache.get(short_code)

        if long_url is None:
            entity = self._repository.find_by_code(short_code)
            if entity is None:
                raise UrlNotFoundError(short_code)
            if entity.is_expired(self._clock.now()):
                self._cache.invalidate(short_code)
                raise UrlExpiredError(short_code)
            long_url = entity.long_url
            self._cache.set(short_code, long_url, ttl_seconds=86400)

        self._click_publisher.publish(
            ClickEvent(
                short_code=short_code,
                timestamp=self._clock.now(),
                ip_address=request_ctx.ip_address,
                user_agent=request_ctx.user_agent,
                referrer=request_ctx.referrer,
            )
        )
        return long_url


class AnalyticsService:
    """Reads only — never touches the write path. Segregated interface
    keeps redirect-service callers from depending on analytics-query
    methods they don't need (Interface Segregation)."""

    def __init__(self, analytics_repository: "AnalyticsRepository"):
        self._repo = analytics_repository

    def get_click_count(self, short_code: str) -> int:
        return self._repo.total_clicks(short_code)

    def get_geo_breakdown(self, short_code: str) -> dict:
        return self._repo.clicks_by_country(short_code)

    def get_referrer_breakdown(self, short_code: str) -> dict:
        return self._repo.clicks_by_referrer(short_code)
```

### 3.5 Liskov Substitution in Practice

Any `UrlRepository` implementation must honor the same contract: `save()` is idempotent-safe under retries, `find_by_code()` returns `None` (never throws) for a missing key, and `exists()` never has side effects. This lets `UrlShortenerService` run against `PostgresUrlRepository` in production and `InMemoryUrlRepository` in unit tests with zero behavioral surprises.

```python
class InMemoryUrlRepository(UrlRepository):
    """Test double / local-dev implementation — fully substitutable
    for any production repository implementing the same interface."""

    def __init__(self):
        self._store: dict[str, ShortUrl] = {}

    def save(self, url: ShortUrl) -> None:
        self._store[url.short_code] = url

    def find_by_code(self, short_code: str) -> Optional[ShortUrl]:
        return self._store.get(short_code)

    def exists(self, short_code: str) -> bool:
        return short_code in self._store

    def delete_expired(self, before: datetime) -> int:
        expired = [c for c, u in self._store.items() if u.expires_at and u.expires_at < before]
        for c in expired:
            del self._store[c]
        return len(expired)


class ShardedPostgresUrlRepository(UrlRepository):
    """Production implementation: routes reads/writes to the correct
    shard by hashing short_code. Same interface, swapped freely."""

    def __init__(self, shard_router: "ShardRouter"):
        self._router = shard_router

    def save(self, url: ShortUrl) -> None:
        conn = self._router.connection_for(url.short_code)
        # INSERT ... ON CONFLICT DO NOTHING (idempotent write)

    def find_by_code(self, short_code: str) -> Optional[ShortUrl]:
        conn = self._router.connection_for(short_code)
        # SELECT ... WHERE short_code = %s
        ...

    def exists(self, short_code: str) -> bool: ...
    def delete_expired(self, before: datetime) -> int: ...
```

### 3.6 SOLID Principle Summary

| Principle                 | Where applied                                                                                                                                                                         |
| ------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **S**ingle Responsibility | `UrlShortenerService` (creation), `RedirectService` (resolution), `AnalyticsService` (querying), `AliasPolicy` (naming rules) are all separate classes with one reason to change each |
| **O**pen/Closed           | New `ShortCodeGenerator` or `DistributedIdGenerator` strategies (e.g. adding a KGS-based generator) can be added without editing existing services                                    |
| **L**iskov Substitution   | Every `UrlRepository` / `CacheStore` implementation honors the same pre/post-conditions and is interchangeable (Postgres ↔ in-memory ↔ DynamoDB)                                      |
| **I**nterface Segregation | `ClickEventPublisher` (write-only) and `AnalyticsRepository` (read-only) are separate interfaces rather than one bloated `AnalyticsStore`                                             |
| **D**ependency Inversion  | Services depend on abstract `ABC` ports, injected via constructor; concrete infra (Redis, Postgres, Kafka) is chosen at composition-root/DI-container level                           |

---

## 4. Short Code Generation: Algorithm Choice

| Approach                                       | Uniqueness guarantee                | Coordination needed                                         | Notes                                                                     |
| ---------------------------------------------- | ----------------------------------- | ----------------------------------------------------------- | ------------------------------------------------------------------------- |
| **Counter + base62 (Snowflake/range-handout)** | By construction                     | Low (Snowflake: none; range-handout: periodic ticket fetch) | **Recommended** — no collision checks, predictable 7-char length          |
| Random string + collision check                | Probabilistic                       | DB read per attempt                                         | Simpler but adds a read on the write path; retry storms possible at scale |
| MD5/SHA hash of URL + truncation               | Probabilistic (birthday collisions) | DB read per attempt                                         | Nice property: same URL → same code (until collision retry breaks that)   |

**Recommendation:** distributed ID generator (Snowflake-style) → base62 encode. This removes collision-checking from the write path entirely, keeping shorten latency low and predictable (N1) even as write volume grows (F6).

---

## 5. Data Model

### 5.1 URL Metadata (OLTP — sharded relational or wide-column store)

```
Table: urls
┌─────────────────┬──────────────┬────────────────────────────────┐
│ Column           │ Type          │ Notes                            │
├─────────────────┼──────────────┼────────────────────────────────┤
│ short_code (PK)  │ VARCHAR(10)   │ Sharded/partitioned on this key  │
│ long_url          │ TEXT          │                                   │
│ owner_id          │ VARCHAR(36)   │ nullable, FK -> users             │
│ created_at        │ TIMESTAMP     │                                   │
│ expires_at        │ TIMESTAMP     │ nullable = never expires          │
│ is_custom_alias   │ BOOLEAN       │                                   │
│ status            │ ENUM          │ ACTIVE | EXPIRED | DISABLED       │
└─────────────────┴──────────────┴────────────────────────────────┘
```

- **Partition key:** `short_code` (consistent hashing across shards) — even distribution, and both read and write paths key off it, so no cross-shard joins are needed for the hot path.
- **Secondary index** on `owner_id` (for "my links" dashboard) can live in a separate lookup table or a search index (Elasticsearch) populated asynchronously, avoiding a hot secondary index on the write-heavy primary table.

### 5.2 Click Analytics (OLAP — columnar store, e.g. ClickHouse/BigQuery/Redshift)

```
Table: click_events
┌─────────────────┬──────────────┬─────────────────────────┐
│ Column           │ Type          │ Notes                     │
├─────────────────┼──────────────┼─────────────────────────┤
│ short_code        │ VARCHAR(10)   │ partition/sort key        │
│ event_time        │ TIMESTAMP     │ partition by day/hour      │
│ ip_address        │ VARCHAR(45)   │                            │
│ geo_country       │ VARCHAR(2)    │ enriched async by GeoIP    │
│ geo_city          │ VARCHAR(100)  │                            │
│ user_agent        │ TEXT          │                            │
│ device_type       │ VARCHAR(20)   │ parsed from UA             │
│ referrer          │ TEXT          │ nullable                   │
└─────────────────┴──────────────┴─────────────────────────┘
```

Kept entirely separate from the OLTP `urls` table because its access pattern (append-heavy, aggregate-query-heavy) and volume (billions of rows, far exceeding URL count) demand different storage engines. Aggregates (daily click counts) are periodically rolled up into a smaller summary table for fast dashboard reads.

---

## 6. API Design

```
POST   /api/v1/urls
       Body: { "long_url": "...", "custom_alias": "optional", "ttl_days": 30 }
       201 -> { "short_code": "aZ9kLm2", "short_url": "https://sho.rt/aZ9kLm2", "expires_at": "..." }
       409 -> alias already taken
       422 -> invalid URL

GET    /{short_code}
       302 -> Location: <long_url>
       404 -> not found
       410 -> expired

GET    /api/v1/urls/{short_code}/analytics?from=...&to=...
       200 -> { "total_clicks": 1523, "by_country": {...}, "by_referrer": {...}, "by_day": [...] }

DELETE /api/v1/urls/{short_code}
       204 -> deleted (owner only)
```

---

## 7. Scalability & Reliability

### 7.1 Read path (redirect) — the hot path

- **Multi-tier caching:** CDN/edge cache for the most popular links → Redis cluster (sharded, replicated) → database. Given a typical 80/20 access pattern, a well-sized cache absorbs the vast majority of the 400K/sec peak reads.
- **Cache eviction:** LRU with TTL slightly shorter than link expiry, so stale-after-expiry reads are naturally bounded.
- **Redis high availability:** Redis Cluster with replica shards; automatic failover via Sentinel/Cluster mode.

### 7.2 Write path (shorten)

- Stateless shorten-service instances behind a load balancer, horizontally scaled.
- ID generation is local (Snowflake) or amortized (range-handout), so writes don't serialize through a single counter.
- Database sharded by `short_code` hash — writes distribute evenly across shards.

### 7.3 Database scaling

- **Sharding:** consistent hashing on `short_code` across N shards; a lightweight `ShardRouter` (consulted by the repository layer, §3.5) maps a code to its shard, with re-sharding supported via virtual nodes to avoid a full data reshuffle when adding shards.
- **Replication:** each shard has a primary + N read replicas across availability zones. Redirect reads can go to replicas; writes go to primaries.
- **Multi-region:** primary region handles writes; async cross-region replication feeds read replicas near users, reducing redirect latency globally. Conflict risk is minimal since `short_code` is globally unique at generation time (no cross-region write conflicts on the same key in the common case).

### 7.4 Fault tolerance

- **No single point of failure:** every tier (LB, service instances, cache, DB shard, message queue) is deployed with N+2 redundancy across at least 3 AZs.
- **Circuit breakers** between redirect-service and cache/DB, so a slow dependency degrades gracefully (e.g., serve from a slightly stale replica) rather than cascading failures.
- **Async analytics pipeline (Kafka)** means a downstream analytics outage never blocks or slows redirects — click events queue and are processed once the consumer recovers, satisfying N1/N2 even under partial failure.
- **Health checks & auto-replacement** of unhealthy instances at the load-balancer/orchestrator level (e.g., Kubernetes liveness probes).

### 7.5 Consistency model

| Data                            | Consistency requirement                | Approach                                                                                                                                                             |
| ------------------------------- | -------------------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Short code uniqueness           | **Strong**                             | Generation guarantees uniqueness by construction (Snowflake/range-handout), or a conditional write (`INSERT ... ON CONFLICT`) for custom aliases at the owning shard |
| URL metadata (long_url, expiry) | **Strong on write, eventual on cache** | Primary DB is source of truth; cache invalidated on update/delete, brief staleness window acceptable                                                                 |
| Click analytics                 | **Eventual**                           | Async event pipeline; a click may take seconds to appear in dashboards — acceptable per N4                                                                           |

This mirrors the classic pattern: the parts of the system where incorrectness is user-visible and unacceptable (duplicate short codes, broken redirects) get strong consistency; the parts where a few seconds of staleness is invisible to the end user (analytics counters) trade consistency for throughput and availability (per CAP, favoring **A**vailability + **P**artition tolerance for analytics, and **C**onsistency + **P**artition tolerance for the core URL mapping).

### 7.6 Rate limiting & abuse prevention

- Token-bucket rate limiter at the API gateway, per API key / IP, protects the shorten endpoint from abuse and enforces fair use at scale.
- URL validator (§3.1, `UrlValidator`) checks against a denylist/threat-intel feed before persisting, preventing the service from being used to shorten malicious links.

---

## 8. Class Diagram (textual)

```
┌──────────────────────┐        ┌───────────────────────┐
│  ShortCodeGenerator    │◄─┐    │  DistributedIdGenerator │
│  <<interface>>          │  │    │  <<interface>>            │
└──────────────────────┘  │    └───────────────────────┘
        ▲                  │              ▲
        │ implements        │              │ implements
┌───────┴────────┐  ┌──────┴───────┐ ┌─────┴──────────────┐
│CounterBasedGen  │  │HashBasedGen   │ │SnowflakeIdGenerator  │
└─────────────────┘  └───────────────┘ │RangeHandoutIdGenerator│
                                        └──────────────────────┘

┌──────────────────────┐   uses    ┌────────────────────┐
│  UrlShortenerService   │──────────►│ ShortCodeGenerator   │
│                          │──────────►│ UrlRepository <<i>>  │
│                          │──────────►│ CacheStore <<i>>     │
│                          │──────────►│ UrlValidator <<i>>   │
│                          │──────────►│ AliasPolicy <<i>>    │
└──────────────────────┘           └────────────────────┘

┌──────────────────────┐   uses    ┌────────────────────┐
│  RedirectService        │──────────►│ UrlRepository <<i>>  │
│                          │──────────►│ CacheStore <<i>>     │
│                          │──────────►│ ClickEventPublisher  │
└──────────────────────┘           └────────────────────┘

┌──────────────────────┐   uses    ┌────────────────────┐
│  AnalyticsService       │──────────►│ AnalyticsRepository  │
└──────────────────────┘           └────────────────────┘

UrlRepository <<interface>>
        ▲
        │ implements
┌───────┴─────────────────┐   ┌─────────────────────┐
│ InMemoryUrlRepository      │   │ ShardedPostgresUrlRepo │
└───────────────────────────┘   └─────────────────────┘
```

---

## 9. Summary

- **Split hot/cold paths:** redirect (read-heavy, latency-critical) and shorten (write path) are independently scaled services; analytics is fully decoupled via async messaging.
- **ID generation by construction** avoids collision-checking on the write path, keeping shorten latency low at billions-of-rows scale.
- **Cache-aside with multi-tier caching** keeps redirect latency minimal without sacrificing correctness.
- **Sharded, replicated, multi-AZ storage** with clear consistency boundaries (strong for the URL mapping, eventual for analytics) satisfies both the reliability and consistency non-functional requirements.
- **SOLID-driven OOP design** — small interfaces (`ShortCodeGenerator`, `UrlRepository`, `CacheStore`, `ClickEventPublisher`) injected into focused services (`UrlShortenerService`, `RedirectService`, `AnalyticsService`) — makes every infrastructure choice (Postgres vs. DynamoDB, Redis vs. Memcached, Snowflake vs. range-handout IDs) swappable without touching business logic, and keeps the system unit-testable end to end.
