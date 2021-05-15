#include "Sanji.hpp"
#include <vector>

namespace sanji_ {

/* Type definitions */
template <typename T>
using vector = std::vector<T>;

/* Extern declarations */
extern int                gbl_current_fig_index;
extern vector<sh_fig_ptr> gbl_fig_holder;
extern vector<bool>       gbl_free_fig_indices;

};

namespace sanji {

using namespace sanji_;

void init() {
    /* Initialize global variables */
    gbl_current_fig_index = -1;
    gbl_free_fig_indices  = vector<bool>(10,true);
    gbl_fig_holder.resize(10);
}

sh_fig_ptr figure() {
    return figure("");
}

sh_fig_ptr figure(const QString fig_name) {
    const sh_fig_ptr fig_ptr = std::make_shared<Figure>(fig_name);
    gbl_fig_holder[gbl_current_fig_index] = fig_ptr;
    return fig_ptr;
}

void plot(const VectorXd& x, const MatrixXd& y, const int priority, const char line_style, const Color color) {
    // Make sure that a figure is available
    if (gbl_current_fig_index == -1) figure();

    // Plot the data
    gbl_fig_holder[gbl_current_fig_index]->plot(x,y,priority,line_style,color);
}

};
