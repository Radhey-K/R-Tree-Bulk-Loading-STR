# STR method for bulk-loading R-trees
Bulk loading points present in specified file into an R Tree using STR method and traversing the tree using pre order traversal. 

How to run: 
```bash
gcc bulk_loading.c -lm
./a.out <filename>
```

Data in input file must be of format 
```
<x_coord> <y_coord>
```