#include <Rcpp.h>
using namespace Rcpp;

// [[Rcpp::export]]
IntegerVector doCellFromXY(
    int ncols, int nrows, double xmin, double xmax, double ymin, double ymax,
    NumericVector x, NumericVector y) {
  
  size_t len = x.size();
  
  double yres_inv = nrows / (ymax - ymin);
  double xres_inv = ncols / (xmax - xmin);
  
  IntegerVector result(len);
  
  for (size_t i = 0; i < len; i++) {
    double row = (ymax - y[i]) * yres_inv;
    double col = (x[i] - xmin) * xres_inv;
    if (row < 0 || row >= nrows || col < 0 || col >= ncols) {
      result[i] = NA_INTEGER;
    } else {
      result[i] = static_cast<int>(row) * ncols + static_cast<int>(col) + 1;
    }
  }
  
  return result;
}

// [[Rcpp::export]]
NumericMatrix doXYFromCell(
    int ncols, int nrows, double xmin, double xmax, double ymin, double ymax,
    IntegerVector cell
) {
  size_t len = cell.size();
  
  double yres = (ymax - ymin) / nrows;
  double xres = (xmax - xmin) / ncols;
  
  NumericMatrix result(len, 2);
  
  for (size_t i = 0; i < len; i++) {
    int c = cell[i] - 1;
    size_t col = c % ncols;
    size_t row = (c / ncols);
    result(i,0) = (col + 0.5) * xres + xmin;
    result(i,1) = -((row + 0.5) * yres - ymax);
  }
  
  return result;
}

size_t oneBasedRowColToCellNum(int ncols, int row, int col) {
	return (row-1) * ncols + col;
}

// [[Rcpp::export]]
IntegerMatrix doFourCellsFromXY(
		int ncols, int nrows, double xmin, double xmax, double ymin, double ymax,
		NumericMatrix xy, bool duplicates, bool isGlobalLonLat
) {
	size_t len = xy.nrow();

	double yres_inv = nrows / (ymax - ymin);
	double xres_inv = ncols / (xmax - xmin);
	
	IntegerMatrix result(len, 4);
	
	for (size_t i = 0; i < len; i++) {
		// 1-based row and col. The 0.5 is because rows/cells are addressed by their
		// centers, not by their bottom/left edges.
		double row = (ymax - xy(i,1)) * yres_inv + 0.5;
		double col = (xy(i,0) - xmin) * xres_inv + 0.5;
		
		double roundRow = round(row);
		double roundCol = round(col);
		
		// Check for out-of-bounds.
		if (roundRow < 1 || roundRow > nrows || roundCol < 1 || roundCol > ncols) {
			result(i,0) = NA_INTEGER;
			result(i,1) = NA_INTEGER;
			result(i,2) = NA_INTEGER;
			result(i,3) = NA_INTEGER;
			continue;
		}
		
		// roundRow and roundCol are now the nearest row/col to x/y.
		// That gives us one corner. We will find the other corner by starting
		// at roundRow/roundCol and moving in the direction of row/col, stopping
		// at the next integral values.
		
		// >0 if row is greater than the nearest round row, 0 if equal
		double vertDir = row - roundRow;
		// >0 if col is greater than the nearest round col, 0 if equal
		double horizDir = col - roundCol;

		// If duplicates are not allowed, make sure vertDir and horizDir
		// are not 0
		if (!duplicates) {
			if (vertDir == 0)
				vertDir = 1;
			if (horizDir == 0)
				horizDir = 1;
		}
		
		// roundRow and roundCol will be one corner; posRow and posCol will be
		// the other corner. Start out by moving left/right or up/down relative
		// to roundRow/roundCol.
		double posRow = roundRow + (vertDir > 0 ? 1 : vertDir < 0 ? -1 : 0);
		double posCol = roundCol + (horizDir > 0 ? 1 : horizDir < 0 ? -1 : 0);
		
		// Now, some fixups in case posCol/posRow go off the edge of the raster.
		
		if (isGlobalLonLat) {
			if (posCol < 1) {
				posCol = ncols;
			} else if (posCol > ncols) {
				posCol = 1;
			}
		} else {
			if (posCol < 1) {
				posCol = 2;
			} else if (posCol > ncols) {
				posCol = ncols - 1;
			}
		}
		
		if (posRow < 1) {
			posRow = 2;
		} else if (posRow > nrows) {
			posRow = nrows - 1;
		}
		
		// Fixups done--just store the results.

		result(i,0) = oneBasedRowColToCellNum(ncols, roundRow, roundCol);
		result(i,1) = oneBasedRowColToCellNum(ncols, posRow, roundCol);
		result(i,2) = oneBasedRowColToCellNum(ncols, posRow, posCol);
		result(i,3) = oneBasedRowColToCellNum(ncols, roundRow, posCol);
	}
	
	return result;
}