#ifndef VENTANAPRINCIPAL_H
#define VENTANAPRINCIPAL_H

#include <QMainWindow>
#include "QProcess"
#include "QSettings"

namespace Ui {
class VentanaPrincipal;
}

class VentanaPrincipal : public QMainWindow
{
    Q_OBJECT

public:
    explicit VentanaPrincipal(QWidget *parent = nullptr);
    ~VentanaPrincipal();
    QString contrasena;




private slots:
    void iniciar();
    void detener();
    void refrescarLista();
    void refrescarInterfaces();
    void mostrarContrasena(int);
    void repararWifi();
    void comprobarVirtual(int);
    void verClientes();
    void verificarContrasena();
    void verificarMac();

private:
    Ui::VentanaPrincipal *ui;
};

#endif // VENTANAPRINCIPAL_H
