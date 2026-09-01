package storage

import (
	"fmt"
	"sync"
	"urlshortner/models"
)

type Storage interface {
	Store(models.UrlMapper) error
	Get(string) (models.UrlMapper, error)
}

type InMemStorage struct {
	shortToMapper map[string]models.UrlMapper
	mu            sync.RWMutex
}

func NewInMemStorage() *InMemStorage {
	return &InMemStorage{
		shortToMapper: make(map[string]models.UrlMapper),
	}
}

func (this *InMemStorage) Store(data models.UrlMapper) error {
	this.mu.Lock()
	defer this.mu.Unlock()
	shortCode := data.GetShortCode()
	if _, ok := this.shortToMapper[shortCode]; ok {
		return fmt.Errorf("short code already exist")
	}
	this.shortToMapper[shortCode] = data
	return nil
}

func (this *InMemStorage) Get(shortCode string) (models.UrlMapper, error) {
	this.mu.RLock()
	defer this.mu.RUnlock()
	data, ok := this.shortToMapper[shortCode]
	if !ok {
		return models.UrlMapper{}, fmt.Errorf("code doesn't exist")
	}

	return data, nil
}

type DbStorage struct {
	// db instacne
}

func (this *DbStorage) Store(data models.UrlMapper) error {
	//implementing store logic for db here
	return nil
}

func (this *DbStorage) Get(string) (models.UrlMapper, error) {
	//implement logic here
	return models.UrlMapper{}, nil
}
