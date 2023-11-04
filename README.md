# OCR Sudoku Solver

OCR Sudoku Solver by Team Prague

## Build

- Make sure you've installed ``make``, ``gcc``, ``sdl2``, ``sdl2_image`` and ``gtk3``

- Clone the repo

- Execute ``make`` at the root of the repository

## Usage

### SudokuSolver
```sh
./solver grille_00     # Solved grid is in a new created file grille00.result
```

### NeuralNetwork
```sh
./NeuralNetwork --reset     # reset the neural network
./NeuralNetwork --train     # train the neural network on xor function
./NeuralNetwork --predict   # see the results of neural network on different inputs
```
### Image/ImageToCells
```sh
./ImageToCells grille.png     # cells are saved in the directory named Cells/
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
./GridDetection ../ImageExamples/image_01.jpeg
```
