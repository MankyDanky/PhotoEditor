#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_selectImageButton_clicked();

    void on_blurIntensitySlider_valueChanged(int value);

    void on_blurButton_clicked();

    void on_selectNewImageButton_clicked();

    void on_sharpenIntensitySlider_valueChanged(int value);

    void on_sharpenButton_clicked();

    void on_effectsListWidget_currentRowChanged(int currentRow);

    void on_contrastIntensitySlider_valueChanged(int value);

    void on_contrastButton_clicked();

    void on_saveButton_clicked();

    void on_exposureIntensitySlider_valueChanged(int value);

    void on_exposureButton_clicked();

    void on_brightnessIntensitySlider_valueChanged(int value);

    void on_brightnessButton_clicked();

    void on_pixelateIntensitySlider_valueChanged(int value);

    void on_pixelateButton_clicked();

    void on_invertButton_clicked();

    void on_monochromeButton_clicked();

    void on_revertButton_clicked();

private:
    Ui::MainWindow *ui;
    void resizeEvent(QResizeEvent *event = nullptr);
};
#endif // MAINWINDOW_H
