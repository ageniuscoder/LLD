package shortner

import (
	"urlshortner/models"
	gen "urlshortner/shortcodegen"
)

type ShortnerService struct {
	codegenrator gen.ShortCodeGen
}

func NewShortnerService(codegenrator gen.ShortCodeGen) *ShortnerService {
	return &ShortnerService{
		codegenrator,
	}
}

func (s *ShortnerService) ShortUrl(url string) (models.UrlMapper, error) {
	code, err := s.codegenrator.GenShortCode(url)
	if err != nil {
		return models.UrlMapper{}, err
	}
	return models.NewUrlMapper(url, code), nil
}
