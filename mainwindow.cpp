#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QImage>
#include <QPixmap>
#include <QFileDialog>

using namespace std;

QImage image;
QImage originalImage;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Setup effects list
    ui->effectsListWidget->addItem("Blur");
    ui->effectsListWidget->addItem("Sharpen");
    ui->effectsListWidget->addItem("Contrast");
    ui->effectsListWidget->addItem("Exposure");
    ui->effectsListWidget->addItem("Brightness");
    ui->effectsListWidget->addItem("Pixelate");
    ui->effectsListWidget->addItem("Invert");
    ui->effectsListWidget->addItem("Monochrome");
    ui->effectsListWidget->setCurrentRow(0);
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    ui->imageLabel->setPixmap(QPixmap::fromImage(image).scaled(ui->imageLabel->width(), ui->imageLabel->width(), Qt::KeepAspectRatio));
    ui->originalImageLabel->setPixmap(QPixmap::fromImage(originalImage).scaled(ui->imageLabel->width(), ui->imageLabel->width(), Qt::KeepAspectRatio));
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Select image function
void selectImage(QLabel *imageLabelPtr, QLabel *originalImageLabelPtr, QMainWindow *mainWindowPtr, QStackedWidget *stackedWidgetPtr) {
    QString path = QFileDialog::getOpenFileName(mainWindowPtr, "Open Image", "/home", "Image Files (*.png *.jpg *.jpeg *.bmp)");
    if (!path.isNull()) {
        image.load(path);
        image.convertTo(QImage::Format_RGB32);
        originalImage = image.copy();
        imageLabelPtr->setPixmap(QPixmap::fromImage(image).scaled(imageLabelPtr->width(), imageLabelPtr->width(), Qt::KeepAspectRatio));
        originalImageLabelPtr->setPixmap(QPixmap::fromImage(originalImage).scaled(imageLabelPtr->width(), imageLabelPtr->width(), Qt::KeepAspectRatio));
        stackedWidgetPtr->setCurrentIndex(1);
    }
}

// Blur image function
void blurImage(QImage &imageRef, const int &repeats) {
    for (int r = 0; r < repeats; r++) {
        QImage referenceImg = imageRef.copy();
        for (int i = 1; i < imageRef.width() - 1; i++) {
            for (int j = 1; j < imageRef.height() - 1; j++) {
                QColor color = referenceImg.pixelColor(i, j);
                QColor aboveColor = referenceImg.pixelColor(i, j-1);
                QColor belowColor = referenceImg.pixelColor(i, j+1);
                QColor leftColor = referenceImg.pixelColor(i-1, j);
                QColor rightColor = referenceImg.pixelColor(i+1, j);
                int red = (int)(color.red() + aboveColor.red() + belowColor.red() + leftColor.red() + rightColor.red())/5;
                int green = (int)(color.green() + aboveColor.green() + belowColor.green() + leftColor.green() + rightColor.green())/5;
                int blue = (int)(color.blue() + aboveColor.blue() + belowColor.blue() + leftColor.blue() + rightColor.blue())/5;
                int alpha = (int)(color.alpha() + aboveColor.alpha() + belowColor.alpha() + leftColor.alpha() + rightColor.alpha())/5;
                imageRef.setPixel(i, j, qRgba(red, green, blue, alpha));
            }
        }
    }
}

// Select image
void MainWindow::on_selectImageButton_clicked()
{
    selectImage(ui->imageLabel, ui->originalImageLabel, this, ui->stackedWidget);
}

// update blur intensity label
void MainWindow::on_blurIntensitySlider_valueChanged(int value)
{
    ui->blurIntensityLabel->setText(QString::number(value));
}

// Blur image
void MainWindow::on_blurButton_clicked()
{
    blurImage(image, ui->blurIntensitySlider->value());

    ui->imageLabel->setPixmap(QPixmap::fromImage(image).scaled(ui->imageLabel->width(), ui->imageLabel->height(), Qt::KeepAspectRatio));
}

// Select new image
void MainWindow::on_selectNewImageButton_clicked()
{
    selectImage(ui->imageLabel, ui->originalImageLabel, this, ui->stackedWidget);
}

// Update sharpness intensity label
void MainWindow::on_sharpenIntensitySlider_valueChanged(int value)
{
    ui->sharpenIntensityLabel->setText(QString::number(value));
}

// Sharpen image
void MainWindow::on_sharpenButton_clicked()
{
    int intensity = ui->sharpenIntensitySlider->value();
    QImage blurred = image.copy();
    blurImage(blurred, intensity);
    QImage details = image.copy();

    // Create details map
    for (int i = 0; i < image.width(); i++) {
        for (int j = 0; j < image.height(); j++) {
            QColor color = image.pixelColor(i, j);
            QColor blurredColor = blurred.pixelColor(i, j);
            details.setPixel(i, j, qRgba(abs(color.red() - blurredColor.red()), abs(color.green() - blurredColor.green()), abs(color.blue() - blurredColor.blue()), abs(color.alpha() - blurredColor.alpha())));
        }
    }

    // Add details map to original
    for (int i = 0; i < image.width(); i++) {
        for (int j = 0; j < image.height(); j++) {
            QColor color = image.pixelColor(i, j);
            QColor detailsColor = details.pixelColor(i, j);
            image.setPixel(i, j, qRgba(clamp(color.red() + detailsColor.red(), 0, 255), clamp(color.green() + detailsColor.green(), 0, 255), clamp(color.blue() + detailsColor.blue(), 0, 255), clamp(color.alpha() + detailsColor.alpha(), 0, 255)));
        }
    }

    ui->imageLabel->setPixmap(QPixmap::fromImage(image).scaled(ui->imageLabel->width(), ui->imageLabel->height(), Qt::KeepAspectRatio));
}

// Update effects stacked widget when new effect is selected
void MainWindow::on_effectsListWidget_currentRowChanged(int currentRow)
{
    ui->effectsStackedWidget->setCurrentIndex(currentRow);
}

// Update contrast intensity label
void MainWindow::on_contrastIntensitySlider_valueChanged(int value)
{
    ui->contrastIntensityLabel->setText(QString::number(value));
}

// Contrast image
void MainWindow::on_contrastButton_clicked()
{
    float multiplier = 1 + ui->contrastIntensitySlider->value()/100.0;
    for (int i = 0; i < image.width(); i++) {
        for (int j = 0; j < image.height(); j++) {
            QColor color = image.pixelColor(i, j);
            float r = clamp((int)((color.red() - 127.5) * multiplier + 127.5), 0, 255);
            float g = clamp((int)((color.green() - 127.5) * multiplier + 127.5), 0, 255);
            float b = clamp((int)((color.blue() - 127.5) * multiplier + 127.5), 0, 255);
            image.setPixel(i, j, qRgba(r, g, b, color.alpha()));
        }
    }

    ui->imageLabel->setPixmap(QPixmap::fromImage(image).scaled(ui->imageLabel->width(), ui->imageLabel->height(), Qt::KeepAspectRatio));
}

// Save image
void MainWindow::on_saveButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save File", "untitled", "Portable Network Graphics Format (*.png);; Joint Photographic Experts Group Format (*.jpg);; Bitmap Format (*.bmp)");
    image.save(fileName);
}

// Update exposure intensity label
void MainWindow::on_exposureIntensitySlider_valueChanged(int value)
{
    ui->exposureIntensityLabel->setText(QString::number(value));
}

// Expose image
void MainWindow::on_exposureButton_clicked()
{
    int exposureIntensity = ui->exposureIntensitySlider->value();
    for (int i = 0; i < image.width(); i++) {
        for (int j = 0; j < image.height(); j++) {
            int h = 0, s = 0, v = 0;
            QColor color = image.pixelColor(i, j);
            color.getHsv(&h, &s, &v);
            v = clamp((v + exposureIntensity), 0, 255);
            image.setPixelColor(i, j, QColor::fromHsv(h, s, v, color.alpha()));
        }
    }

    ui->imageLabel->setPixmap(QPixmap::fromImage(image).scaled(ui->imageLabel->width(), ui->imageLabel->height(), Qt::KeepAspectRatio));
}

// Update brightenss intensity label
void MainWindow::on_brightnessIntensitySlider_valueChanged(int value)
{
    ui->brightnessIntensityLabel->setText(QString::number(value));
}

// Brighten image
void MainWindow::on_brightnessButton_clicked()
{
    int brightnessIntensity = ui->brightnessIntensitySlider->value();
    for (int i = 0; i < image.width(); i++) {
        for (int j = 0; j < image.height(); j++) {
            int h = 0, s = 0, l = 0;
            QColor color = image.pixelColor(i, j);
            color.getHsl(&h, &s, &l);
            l = clamp((l + brightnessIntensity), 0, 255);
            image.setPixelColor(i, j, QColor::fromHsl(h, s, l, color.alpha()));
        }
    }

    ui->imageLabel->setPixmap(QPixmap::fromImage(image).scaled(ui->imageLabel->width(), ui->imageLabel->height(), Qt::KeepAspectRatio));
}

// Update pixelate intensity label
void MainWindow::on_pixelateIntensitySlider_valueChanged(int value)
{
    ui->pixelateIntensityLabel->setText(QString::number(value));
}

// Pixelate image
void MainWindow::on_pixelateButton_clicked()
{
    int pixelateIntensity = ui->pixelateIntensitySlider->value();
    for (int i = 0; i < image.width(); i+=pixelateIntensity) {
        for (int j = 0; j < image.height(); j+=pixelateIntensity) {
            QRgb color = image.pixel(i, j);
            for (int k = 0; k < pixelateIntensity; k++) {
                if (k+i >= image.width()) break;
                for (int l = 0; l < pixelateIntensity; l++) {
                    if (j+l >= image.height()) break;
                    image.setPixel(i+k, j+l, color);
                }
            }
        }
    }

    ui->imageLabel->setPixmap(QPixmap::fromImage(image).scaled(ui->imageLabel->width(), ui->imageLabel->height(), Qt::KeepAspectRatio));
}

// Invert image
void MainWindow::on_invertButton_clicked()
{
    for (int i = 0; i < image.width(); i++) {
        for (int j = 0; j < image.height(); j++) {
            QColor color = image.pixelColor(i, j);
            image.setPixel(i, j, qRgba(255-color.red(), 255-color.green(), 255-color.blue(), color.alpha()));
        }
    }

    ui->imageLabel->setPixmap(QPixmap::fromImage(image).scaled(ui->imageLabel->width(), ui->imageLabel->height(), Qt::KeepAspectRatio));
}

// Monochrome image
void MainWindow::on_monochromeButton_clicked()
{
    for (int i = 0; i < image.width(); i++) {
        for (int j = 0; j < image.height(); j++) {
            QColor color = image.pixelColor(i, j);
            int grayscale = color.red()*0.299 + color.green()*0.587 + color.blue()*0.114;
            image.setPixel(i, j, qRgba(grayscale, grayscale, grayscale, color.alpha()));
        }
    }

    ui->imageLabel->setPixmap(QPixmap::fromImage(image).scaled(ui->imageLabel->width(), ui->imageLabel->height(), Qt::KeepAspectRatio));
}

// Revert image
void MainWindow::on_revertButton_clicked()
{
    image = originalImage.copy();
    ui->imageLabel->setPixmap(QPixmap::fromImage(image).scaled(ui->imageLabel->width(), ui->imageLabel->height(), Qt::KeepAspectRatio));
}
