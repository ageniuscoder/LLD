package visitor

import "fmt"

type Visitor interface {
	VisitText(*Text)
	VisitImage(*Image)
}

type CalcSize struct{}

func NewCalcSize() CalcSize {
	return CalcSize{}
}

func (c CalcSize) VisitText(t *Text) {
	fmt.Println("Text size Calculator, size: ", t.size)
}

func (c CalcSize) VisitImage(i *Image) {
	fmt.Println("Image Size Calculator")
}

type Compress struct{}

func NewCompress() Compress {
	return Compress{}
}

func (c Compress) VisitText(t *Text) {
	fmt.Println("Compressor for text")
}

func (c Compress) VisitImage(i *Image) {
	fmt.Println("compress for Image")
}

type Document interface {
	Accept(Visitor)
}

type Text struct {
	size    int
	content string
}

func NewText(size int, content string) *Text {
	return &Text{
		size,
		content,
	}
}

func (t *Text) Accept(v Visitor) {
	v.VisitText(t)
}

type Image struct {
	size int
}

func NewImage(size int) *Image {
	return &Image{
		size,
	}
}

func (i *Image) Accept(v Visitor) {
	v.VisitImage(i)
}
