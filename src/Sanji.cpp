#include <QFontDatabase>
#include <QDebug>
#include <vector>
#include "Sanji.hpp"

namespace sanji_ {

/* Type definitions */
template <typename T>
using vector        = std::vector<T>;
template <typename K, typename T>
using unordered_map = std::unordered_map<K,T>;

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

void plot(const VectorXd& x, const MatrixXd& y, const Style& style, const int priority) {
    if (x.rows() == 0) return;

    // Make sure that a figure is available
    if (gbl_current_fig_index == -1) figure();

    // Plot the data
    gbl_fig_holder[gbl_current_fig_index]->plot(x,y,style,priority);
}

void imshow(const Image &img) {
    // Make sure that a figure is available
    if (gbl_current_fig_index == -1) figure();

    // Plot the data
    gbl_fig_holder[gbl_current_fig_index]->imshow(img);
}

void quiver(const VectorXd& x, const VectorXd& y, const VectorXd& u, const VectorXd& v, const Style& style, const int priority) {
    if (x.rows() == 0) return;

    // Make sure that a figure is available
    if (gbl_current_fig_index == -1) figure();

    // Plot the data
    gbl_fig_holder[gbl_current_fig_index]->quiver(x,y,u,v,style,priority);
}

void quiver(const VectorXd& x, const VectorXd& y, const VectorXd& u, const VectorXd& v, const Style& style) {
    quiver(x,y,u,v,style,0);
}

void quiver(const VectorXd& x, const VectorXd& y, const VectorXd& u, const VectorXd& v, const Style& style, std::vector<std::string> flags) {
    Style style_ = style;
    for (const auto& s : flags) style_[s] = 1.0;
    quiver(x,y,u,v,style_,{});
}

void printAvailableFontFamilies() {
    const QFontDatabase fdb;
    qInfo() << fdb.families();
}

void setAxesRatio(const std::string& axes_ratio) {
    if (gbl_current_fig_index != -1)
        gbl_fig_holder[gbl_current_fig_index]->setAxesRatio(axes_ratio);
}

void setxmin(const double xmin) {
    if (gbl_current_fig_index != -1)
        gbl_fig_holder[gbl_current_fig_index]->setxmin(xmin);
}

void setxmax(const double xmax) {
    if (gbl_current_fig_index != -1)
        gbl_fig_holder[gbl_current_fig_index]->setxmax(xmax);
}

void setymin(const double ymin) {
    if (gbl_current_fig_index != -1)
        gbl_fig_holder[gbl_current_fig_index]->setymin(ymin);
}

void setymax(const double ymax) {
    if (gbl_current_fig_index != -1)
        gbl_fig_holder[gbl_current_fig_index]->setymax(ymax);
}

void setxlimits(const double xmin, const double xmax) {
    if (gbl_current_fig_index != -1) {
        gbl_fig_holder[gbl_current_fig_index]->setxmin(xmin);
        gbl_fig_holder[gbl_current_fig_index]->setxmax(xmax);
    }
}

void setylimits(const double ymin, const double ymax) {
    if (gbl_current_fig_index != -1) {
        gbl_fig_holder[gbl_current_fig_index]->setymin(ymin);
        gbl_fig_holder[gbl_current_fig_index]->setymax(ymax);
    }
}

void setPlotBackgroundColor(const uint32_t color) {
    if (gbl_current_fig_index != -1)
        gbl_fig_holder[gbl_current_fig_index]->setPlotBackgroundColor(color);
}

void setxTicksBackgroundColor(const uint32_t color) {
    if (gbl_current_fig_index != -1)
        gbl_fig_holder[gbl_current_fig_index]->setxTicksBackgroundColor(color);
}

void setyTicksBackgroundColor(const uint32_t color) {
    if (gbl_current_fig_index != -1)
        gbl_fig_holder[gbl_current_fig_index]->setyTicksBackgroundColor(color);
}

};
