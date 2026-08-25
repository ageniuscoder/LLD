package main

import obs "observer/component"

func main() {
	var striver = obs.NewStriver()
	mangal := obs.NewMangal("mangal", striver)
	striver.Add(mangal)
	striver.Add(obs.NewShailu("shailu", striver))

	striver.UploadVideo("array")

	striver.Remove(mangal)

	striver.UploadVideo("tree")
}
