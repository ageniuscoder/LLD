package gen

import (
	"crypto/md5"
	"encoding/hex"
	"io"
	"sync/atomic"
)

type ShortCodeGen interface {
	GenShortCode(url string) (string, error)
}

type Base62Gen struct {
	counter int64
	codes   string
}

func NewBase62Gen() *Base62Gen {
	return &Base62Gen{
		counter: 0,
		codes:   "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
	}
}

func (this *Base62Gen) nextCounter() int64 {
	return atomic.AddInt64(&this.counter, 1)
}

func (this *Base62Gen) GenShortCode(url string) (string, error) { //reverse base62 encoding
	uniqueId := this.nextCounter()

	code := make([]byte, 0, 7)
	for uniqueId > 0 {
		code = append(code, this.codes[uniqueId%62])
		uniqueId /= 62
	}
	return string(code), nil
}

type Md5Gen struct{}

func NewMd5Gen() *Md5Gen {
	return &Md5Gen{}
}

func (m *Md5Gen) GenShortCode(url string) (string, error) {
	h := md5.New()

	_, err := io.WriteString(h, url)
	if err != nil {
		return "", err
	}

	hashBytes := h.Sum(nil)
	hashStr := hex.EncodeToString(hashBytes)

	return hashStr[:7], nil
}
