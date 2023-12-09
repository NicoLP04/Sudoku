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
