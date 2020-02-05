#include "ventanaprincipal.h"
#include "ui_ventanaprincipal.h"
#include "QStringList"
#include "QDebug"
#include "QTextCodec"
#include "manejo_string.cpp"
#include "QAbstractItemModel"
#include "QInputDialog"
#include "QThread"



VentanaPrincipal::VentanaPrincipal(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::VentanaPrincipal)
{
    QStringList totalCanales2_4={"1","2","3","4","5","6","7","8","9","10","11","12","13","14"};

    QStringList wifi;
    QStringList interfaces;


    QSettings settings("Paketo", "Visual_create_ap");
    ui->setupUi(this);
    ui->ckbCanal->addItems(totalCanales2_4);

    bool ok;
    contrasena = QInputDialog::getText(this, "Autorizacion",
    "Se necesita autorizacion ADMINISTRADOR (sudo) para iniciar algunos procesos\nContraseña:", QLineEdit::Password, "", &ok);
    //if (ok && !text.isEmpty())

    QProcess process1;
    QProcess process2;
    QProcess process3;
    QProcess process4;


    process1.setStandardOutputProcess(&process2);
    process3.setStandardOutputProcess(&process4);

    process1.start("echo "+contrasena);
    process2.start("sudo -S ifconfig");
    process2.setProcessChannelMode(QProcess::ForwardedChannels);

    // Wait for it to start
    if(!process1.waitForStarted()) {
        qDebug() << "No se pueden abrir los procesos";
        return ;
    }
    QByteArray buffer;
    if(!process2.waitForFinished()){
    qDebug() << "Problemas con los procesos";
    return ;
    }
    QByteArray response = process2.readAll();
    process1.close();
    process2.close();
    QString textIfconfig = response.data();
    interfaces=getInterf(textIfconfig);

    process3.start("echo "+contrasena);
    process4.start("sudo -S iwconfig");
    process4.setProcessChannelMode(QProcess::ForwardedChannels);

    // Wait for it to start
    if(!process3.waitForStarted()) {
        qDebug() << "No se pueden abrir los procesos";
        return ;
    }
    if(!process4.waitForFinished()){
    qDebug() << "Problemas con los procesos";
    return ;
    }
    response = process4.readAll();
    process4.close();
    process3.close();
    textIfconfig = response.data();
    qDebug() <<textIfconfig;
    wifi=getInterf(textIfconfig);
    ui->internet->addItems(interfaces);
    ui->wifi->addItems(wifi);
    qDebug() << wifi;
    qDebug()<<interfaces;



/*
    proceso.start("gksudo ifconfig");
    if(!proceso.waitForStarted()){
    qDebug() << "NO RULA";
    return ;
    }
    //bash.closeWriteChannel();
    if(!proceso.waitForFinished()){
    qDebug() << "NO RULA";
    return ;
    }
    QByteArray response = proceso.readAll();
    proceso.close();
    QString textIfconfig = response.data();
    //qDebug()<<textIfconfig;
    interfaces=getInterf(textIfconfig);


    proceso.start("gksudo iwconfig");
    if(!proceso.waitForStarted()){
    qDebug() << "No se pueden abrir los procesos";
    return ;
    }
    //bash.closeWriteChannel();
    if(!proceso.waitForFinished()){
    qDebug() << "Problemas con los procesos";
    return ;
    }
    response = proceso.readAll();
    proceso.close();

    */

    //TOMAR LOS AJUSTES DE LA SECION ANTERIOR


    QString interfazGuardada=settings.value("wifi","wlo1").toString();
    if(wifi.contains(interfazGuardada))
        ui->wifi->setCurrentIndex(wifi.indexOf(interfazGuardada));

    interfazGuardada=settings.value("interfaces","wlo1").toString();
    if(interfaces.contains(interfazGuardada))
        ui->internet->setCurrentIndex(interfaces.indexOf(interfazGuardada));
    ui->ckbCanal->setCurrentIndex(ui->ckbCanal->findText(settings.value("canal","1").toString()));
    ui->leMacClonada->setText(settings.value("mac","62:D9:DF:02:ED:E3").toString());
    ui->ckbOcultarWifi->setChecked(settings.value("ocultarWifi",false).toBool());
    ui->ckbDesabilitarComClientes->setChecked(settings.value("desabilitarComClientes",false).toBool());
    ui->ckbInterfazVirtual->setChecked(settings.value("interfazVirtual",true).toBool());
    ui->leNombrePunto->setText(settings.value("nombrePunto","Mi_Punto").toString());
    ui->leContrasena->setText(settings.value("contrasena","").toString());

    connect(ui->btRefrescar,SIGNAL(clicked()),SLOT(refrescarLista()));
    connect(ui->btSegundoPlano,SIGNAL(clicked()),SLOT(iniciar()));
    connect(ui->btDetenerSegPlano,SIGNAL(clicked()),SLOT(detener()));
    connect(ui->ckbMostrar,SIGNAL(stateChanged(int)),SLOT(mostrarContrasena(int)));
    connect(ui->btRepararWifi,SIGNAL(clicked()),SLOT(repararWifi()));
    connect(ui->wifi,SIGNAL(currentIndexChanged(int)),SLOT(comprobarVirtual(int)));
    connect(ui->internet,SIGNAL(currentIndexChanged(int)),SLOT(comprobarVirtual(int)));
    connect(ui->btClientes,SIGNAL(clicked()),SLOT(verClientes()));
    connect(ui->leContrasena,SIGNAL(editingFinished()),SLOT(verificarContrasena()));
    connect(ui->leMacClonada,SIGNAL(editingFinished()),SLOT(verificarMac()));
    refrescarLista();

}

VentanaPrincipal::~VentanaPrincipal()
{

    QSettings settings("Paketo", "Visual_create_ap");
    settings.setValue("wifi",ui->wifi->currentText());
    settings.setValue("interfaces",ui->internet->currentText());
    settings.setValue("canal",ui->ckbCanal->currentText());
    settings.setValue("mac",ui->leMacClonada->text());
    settings.setValue("ocultarWifi",ui->ckbOcultarWifi->isChecked());
    settings.setValue("desabilitarComClientes",ui->ckbDesabilitarComClientes->isChecked());
    settings.setValue("interfazVirtual",ui->ckbInterfazVirtual->isChecked());
    settings.setValue("nombrePunto",ui->leNombrePunto->text());
    settings.setValue("contrasena",ui->leContrasena->text());




    delete ui;
}
/////////////////////////////////////////////
/// \brief VentanaPrincipal::iniciar
///
void VentanaPrincipal::iniciar(){

    QString modoComexion;
    if(ui->nat->isChecked())
        modoComexion="nat";
    else if(ui->bridge->isChecked())
        modoComexion="bridge";
    else if(ui->bridge->isChecked())
        modoComexion="none";
    else {
        modoComexion="none";
    }

    QString opciones;
    if(ui->ckbOcultarWifi->isChecked())
        opciones.append("--hidden ");
    if(ui->ckbDesabilitarComClientes->isChecked())
        opciones.append("--isolate-clients ");
    if(!ui->ckbInterfazVirtual->isChecked())
        opciones.append("--no-virt ");

    QProcess process1;
    process1.startDetached("/bin/sh", QStringList()<< "-c"
                           <<"echo "
                             +contrasena
                           +" |sudo -S create_ap --daemon -c "
                           +ui->ckbCanal->currentText()+" --mac "
                           +ui->leMacClonada->text()+" -m "
                           + modoComexion+" "
                           +opciones
                           +ui->wifi->currentText()+" "
                           + ui->internet->currentText()+" "
                           +ui->leNombrePunto->text()+" "
                           +(ui->leContrasena->text().isEmpty()?"":ui->leContrasena->text())
                           );

    // Wait for it to start
    if(!process1.waitForStarted()) {
        qDebug() << "No se pueden abrir los procesos";
        return ;
    }

    QByteArray response = process1.readAll();
    QString textIfconfig = response.data();
    //qDebug()<<textIfconfig;
    if(!process1.waitForFinished()){
    qDebug() << "Problemas con los procesos";
    return ;
    }
    process1.close();



}
void VentanaPrincipal::detener(){

    int objeto=0;
    QProcess process1;
    if(ui->lvProcesos->selectedItems().length()==0)
        return;
    QString interfaz = ui->lvProcesos->selectedItems()[0]->text();
    if(interfaz.isEmpty())
        return;
    QStringList interfazList=interfaz.split(" ");

    if(interfazList.length()==3)
        objeto=2;
    else if(interfazList.length()==2)
        objeto=1;
    else {
        return;
    }
    if(interfazList[1].isEmpty())
        return;
    interfazList[objeto].remove("(");
    interfazList[objeto].remove(")");
    process1.startDetached("/bin/sh", QStringList()<< "-c"
                           <<"echo "
                           +contrasena
                           +" |sudo -S create_ap --stop "
                           +interfazList[objeto]);

    // Wait for it to start
    if(!process1.waitForStarted()) {
        return ;
    }

    QByteArray response = process1.readAll();
    QString textIfconfig = response.data();
    //qDebug()<<textIfconfig;

    process1.close();
    QThread::sleep(2);
    refrescarLista();


}

void VentanaPrincipal::refrescarLista(){
    QProcess process1;
    QProcess process2;
    process1.setStandardOutputProcess(&process2);
    process1.start("echo "+contrasena);
    process2.start("sudo -S create_ap --list-running");
    process2.setProcessChannelMode(QProcess::ForwardedChannels);

    // Wait for it to start
    if(!process1.waitForStarted()) {
        qDebug() << "No se pueden abrir los procesos";
        return ;
    }
    QByteArray buffer;
    if(!process2.waitForFinished()){
    qDebug() << "Problemas con los procesos";
    return ;
    }
    QByteArray response = process2.readAll();
    process1.close();
    process2.close();
    QString textIfconfig = response.data();
    QStringList lineas=textIfconfig.split('\n');
    QStringList procesos;
    for(int a=2;a<lineas.length();a++){
        if(!lineas[a].isEmpty())
            procesos.append(lineas[a]);
    }
    qDebug()<<procesos;
    ui->lvProcesos->clear();
    ui->lvProcesos->insertItems(1,procesos);
    process1.close();
    process2.close();
    ui->lvClientes->clear();

    refrescarInterfaces();
}

void VentanaPrincipal::refrescarInterfaces(){
    QProcess process1;
    QProcess process2;
    QProcess process3;
    QProcess process4;

    QStringList wifi;
    QStringList interfaces;


    process1.setStandardOutputProcess(&process2);
    process3.setStandardOutputProcess(&process4);

    process1.start("echo "+contrasena);
    process2.start("sudo -S ifconfig");
    process2.setProcessChannelMode(QProcess::ForwardedChannels);

    // Wait for it to start
    if(!process1.waitForStarted()) {
        return ;
    }
    QByteArray buffer;
    if(!process2.waitForFinished()){
    qDebug() << "Problemas con los procesos";
    return ;
    }
    QByteArray response = process2.readAll();
    process1.close();
    process2.close();
    QString textIfconfig = response.data();
    interfaces=getInterf(textIfconfig);

    process3.start("echo "+contrasena);
    process4.start("sudo -S iwconfig");
    process4.setProcessChannelMode(QProcess::ForwardedChannels);

    // Wait for it to start
    if(!process3.waitForStarted()) {
        return ;
    }
    if(!process4.waitForFinished()){
    qDebug() << "Problemas con los procesos";
    return ;
    }
    response = process4.readAll();
    process4.close();
    process3.close();
    textIfconfig = response.data();
    qDebug() <<textIfconfig;
    wifi=getInterf(textIfconfig);
    QString wifiGuardada=ui->wifi->currentText();
    QString internetGuardada=ui->internet->currentText();
    ui->internet->clear();
    ui->internet->addItems(interfaces);
    ui->wifi->clear();
    ui->wifi->addItems(wifi);
    if(wifi.contains(wifiGuardada))
        ui->wifi->setCurrentIndex(wifi.indexOf(wifiGuardada));
    if(interfaces.contains(internetGuardada))
        ui->internet->setCurrentIndex(interfaces.indexOf(internetGuardada));

}

void VentanaPrincipal::mostrarContrasena(int mostrar){
    if(mostrar==Qt::Checked)
        ui->leContrasena->setEchoMode(QLineEdit::Normal);
    else
       ui->leContrasena->setEchoMode(QLineEdit::Password);
}


void VentanaPrincipal::repararWifi(){
    QProcess process1;
    process1.startDetached("/bin/sh", QStringList()<< "-c"
                           <<"echo "
                           +contrasena
                           +" |sudo -S create_ap  --fix-unmanaged"
                           );

    // Wait for it to start
    if(!process1.waitForStarted()) {
        qDebug() << "No se pueden abrir los procesos";
        return ;
    }

    QByteArray response = process1.readAll();
    QString textIfconfig = response.data();
    //qDebug()<<textIfconfig;
    if(!process1.waitForFinished()){
    qDebug() << "Problemas con los procesos";
    return ;
    }
    process1.close();

    refrescarLista();
}

void VentanaPrincipal::comprobarVirtual(int estado){
    if(ui->wifi->currentText()==ui->internet->currentText()){
        ui->ckbInterfazVirtual->setCheckState(Qt::Checked);
        ui->ckbInterfazVirtual->setEnabled(false);
    }
    else {
        ui->ckbInterfazVirtual->setEnabled(true);
    }
}

void VentanaPrincipal::verClientes(){
    int objeto=0;
    if(ui->lvProcesos->selectedItems().length()==0)
        return;
    QString interfaz = ui->lvProcesos->selectedItems()[0]->text();
    if(interfaz.isEmpty())
        return;
    QStringList interfazList=interfaz.split(" ");

    if(interfazList.length()==3)
        objeto=2;
    else if(interfazList.length()==2)
        objeto=1;
    else {
        return;
    }
    if(interfazList[1].isEmpty())
        return;
    interfazList[objeto].remove("(");
    interfazList[objeto].remove(")");

    QProcess process1;
    QProcess process2;
    process1.setStandardOutputProcess(&process2);
    process1.start("echo "+contrasena);
    process2.start("sudo -S create_ap  --list-clients "
                   +interfazList[objeto]);
    process2.setProcessChannelMode(QProcess::ForwardedChannels);
    // Wait for it to start
    if(!process1.waitForStarted()) {
        qDebug() << "No se pueden abrir los procesos";
        return ;
    }
    QByteArray buffer;
    if(!process2.waitForFinished()){
    qDebug() << "Problemas con los procesos";
    return ;
    }
    QByteArray response = process2.readAll();
    process1.close();
    process2.close();
    QString textIfconfig = response.data();
    QStringList lineas=textIfconfig.split('\n');
    ui->lvClientes->clear();
    ui->lvClientes->insertItems(1,lineas);
    process1.close();
}

void VentanaPrincipal::verificarContrasena(){
    if(ui->leContrasena->text().length()>0 && ui->leContrasena->text().length()<8)
        ui->leContrasena->setText("");
}

//Ejemplo de Mac 62:D9:DF:02:ED:E3
//               01234567890123456
void VentanaPrincipal::verificarMac(){
    QString caracPermitido="0123456789ABCDEFabcdef";
    QString texto=ui->leMacClonada->text();
    for(int a=0;a<17;a++){
        if(a==2 || a==5 || a==8 || a==11 || a==14){
            if(!(texto[a]==':')){
                //return;//reportar la mac incorrecta
                ui->leMacClonada->setFocus();
            }
        }
        else{
            if(!caracPermitido.contains(texto[a])){
                //return; //reportar la mac incorrecta
                ui->leMacClonada->setFocus();
            }
        }

    }

}
