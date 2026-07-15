# BStar-Indexing

BStar-Indexing is a C++ console application for building and operating on a B-star tree-style index over customer records. The project is implemented in Visual Studio and works with pipe-separated text files containing customer data.

## What it does

- Builds an index from a text file of customer records.
- Inserts and deletes records interactively.
- Searches by primary key.
- Searches by first name.
- Prints the current tree structure.
- Supports searching multiple primary keys in one session.

## Data format

Input files must use this structure per line:

```text
C_ID|C_F_NAME|C_L_NAME|C_EMAIL|C_AD_ID
```

Example:

```text
4300000003|Amos|Labree|ALabree@hotmail.com|4300000507
```

Sample datasets are included in the repository:

- `BSTAR-INDEXING/customer20.txt`
- `BSTAR-INDEXING/customer100.txt`
- `BSTAR-INDEXING/customer1000.txt`

## Build

1. Open `BSTAR-INDEXING.sln` in Visual Studio 2022 or newer.
2. Select a configuration such as `Debug | x64`.
3. Build the solution.

## Run

1. Start the console application from Visual Studio.
2. Choose `1` to create an index.
3. Enter the tree order (`m`) between `3` and `10`.
4. Enter the path to one of the sample data files or your own file.

After the index is created, the menu provides options to print, insert, delete, and search records.

## Repository structure

- `BSTAR-INDEXING.sln` - Visual Studio solution.
- `BSTAR-INDEXING/` - C++ project sources and sample data.
- `BSTAR-INDEXING/main.cpp` - main application and tree implementation.
- `BSTAR-INDEXING/customer*.txt` - example input datasets.

## Notes

- The project is currently organized as a single source file, which keeps it easy to run but leaves room for future refactoring.
- A next cleanup step would be to split the tree, data model, and console UI into separate translation units.