# EAN-8 Barcode Generator and Decoder

This project implements a tool for generating and decoding **EAN-8 barcodes**. It validates EAN-8 identifiers, generates barcode images in the PBM format, and can decode barcodes back into their respective identifiers.

---

## Features

- **Barcode Generation**:
    - Generates EAN-8 barcodes based on a valid identifier.
    - Supports customizable dimensions: spacing, bar width, and barcode height.
    - Outputs barcode images in the PBM (Portable Bitmap) format.

- **Identifier Validation**:
    - Verifies the structure of the EAN-8 identifier.
    - Calculates and adjusts the checksum digit if it is invalid or missing.

- **Barcode Decoding**:
    - Decodes PBM barcode images back into their corresponding EAN-8 identifiers.
    - Validates the structure and markers of the barcode.

---

## Requirements

- **C Compiler** (e.g., GCC or Clang)
- Standard C Libraries (stdlib, stdio, string, etc.)

---

## Installation

1. Clone this repository:

   ```bash
   git clone https://github.com/gustapavao/itp-ean8-barcode.git
   cd itp-ean8-barcode
   ```
2. Compile the source code:

   ```bash
   gcc -o generate_barcode .\encoder.c .\functions.c
   ```

---

## Usage

### Barcode Generation

```bash
./generate_barcode <EAN-8 Identifier> <Output File> [Spacing] [Bar Width] [Height]
```

- `<EAN-8 Identifier>`: The 8-digit identifier for the barcode. If the checksum digit (last digit) is invalid or missing, it will be automatically corrected.
- `<Output File>`: The output PBM file where the barcode will be saved (must end with `.pbm`).
- `[Spacing]` (optional): The spacing around the barcode. Default is `4`.
- `[Bar Width]` (optional): The width of the barcode bars. Default is `3`.
- `[Height]` (optional): The height of the barcode. Default is `50`.

**Example**:

```bash
./generate_barcode 12345678 barcode.pbm 5 4 60
```

Output: The barcode is saved as `barcode.pbm`. If the identifier is invalid, it will be corrected, and a message will indicate the adjusted identifier.

---

### Barcode Decoding

A tool for decoding PBM files back into EAN-8 identifiers.

Compile the source code:

   ```bash
   gcc -o decoder .\decoder.c .\functions.c
   ```


---
**Example**:

```bash
./decoder file.pbm
```

## File Structure

- **main.c**: Entry point of the program, handles argument parsing and validation.
- **Utils.c**: Contains core logic for barcode generation, identifier validation, and utility functions.
- **Utils.h**: Header file with function declarations and shared definitions.

---

## PBM File Format

The program generates PBM files in plain text format (`P1`). The PBM file consists of:

- A header with the format (`P1`) and dimensions.
- The barcode pattern encoded as binary pixels (`0` for white, `1` for black).
- Margins (top and bottom) for spacing.

**Example PBM Header**:

```
P1
100 60
```

---

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

---

## Contributing

Contributions are welcome! Feel free to submit issues or pull requests to improve the project.

---

## Author

Developed by `Gustavo Pavão and Randson Thiago`

