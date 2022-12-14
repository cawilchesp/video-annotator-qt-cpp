#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QDialog>

namespace Ui {
class settingsWindow;
}

class settingsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit settingsWindow(QWidget *parent = nullptr);
    ~settingsWindow();

private slots:
    void on_videoFolderPathbtn_clicked();
    void on_resultsFolderPathbtn_clicked();
    void on_fontComboBox_currentFontChanged(const QFont &f);
    void on_thicknessSpinBox_valueChanged(int arg1);
    void on_sizeSpinBox_valueChanged(int arg1);

    void on_textoColorToolButton_clicked();
    void on_actionTextoNegro_triggered();
    void on_actionTextoBlanco_triggered();
    void on_actionTextoAmarillo_triggered();
    void on_actionTextoAzul_triggered();
    void on_actionTextoRojo_triggered();
    void on_actionTextoVerde_triggered();
    void on_actionTextCustomColor_triggered();

    void on_ObjectColorToolButton_clicked();
    void on_actionObjetoNegro_triggered();
    void on_actionObjetoBlanco_triggered();
    void on_actionObjetoAmarillo_triggered();
    void on_actionObjetoAzul_triggered();
    void on_actionObjetoRojo_triggered();
    void on_actionObjetoVerde_triggered();
    void on_actionObjectCustomColor_triggered();

private:
    Ui::settingsWindow *ui;

    // Settings
    int languageSet;                        // Idioma
    QString videoFolder;                    // Ubicación de la carpeta de los archivos de video
    QString resultsFolder;                  // Ubicación de la carpeta donde se guardan los resultados
    int fontSizeDefault;                    // Tamaño de la fuente predeterminada
    QString annotationFontDefault;          // Fuente predeterminada
    QColor annotationCustomColor;
    QColor customColorObject;
    int thickness;
    int temaDefault;                        // Tema de la aplicación
};

#endif // SETTINGSWINDOW_H
