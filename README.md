# OCR Sudoku Solver

OCR Sudoku Solver by Team Prague

## Build

- Make sure you've installed ``make``, ``gcc``, ``sdl2``, ``sdl2_image`` and ``gtk3``

- Clone the repo

- Execute ``make`` at the root of the repository and use the interface

## Usage

### Interface
After executing ``make`` at the root of the repository, you can use the interface using:
```sh
./interface
```
##### Page 1: 
- button "Next step" -> goes to the next step
- button "Choose image" -> open file explorer to choose an image
- button "Solve !" -> do all the steps
- button "Previous step" -> goes to previous step
- button "Save final image" -> open file explorer to save final image

#### Page 2:
- button "Choose image" -> open file explorer to choose an image
- button "Filter image" -> filter the image
- button "Detect lines" -> apply Hough to detect lines
- button "Rotate image" -> rotate image with given input
- button "Solve" -> solve the image

#### Page 3:
- button "Update Modifications" -> Update grid values according to the textboxes

### SudokuSolver
```sh
./solver grille_00     # Solved grid is in a new created file grille00.result
```

### NeuralNetwork
```sh
./NeuralNetwork --predict   # see the results of neural network on different inputs
```

### Image/ImageToCells
```sh
./ImageToCells grille.png     # create a new file grid that contains the grid
```

### Image/ImagePreprocessing
```sh
./PreProcessing ../ImageExamples/image_01.jpeg     # result named image.jpeg
```

### Image/Rotation
```sh
./rotate ../ImageExamples/image_01.jpeg 37     # result named rotated.png
```
### Image/GridDetection
```sh
./GridDetection <a preprocessed image>    # create cropped image in grid.png
```
