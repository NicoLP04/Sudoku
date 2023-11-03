TOPTARGETS := all clean

SUBDIRS := ./SudokuSolver/ ./NeuralNetwork/ ./Image/GridDetection/ ./Image/ImagePreprocessing/ ./Image/ImageToCells/ ./Image/Rotation/

$(TOPTARGETS): $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

.PHONY: $(TOPTARGETS) $(SUBDIRS)
