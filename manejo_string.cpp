#include "QString"
#include "QStringList"
#include "QDebug"

QStringList getInterf(QString textIfconfig){
    QStringList lineas=textIfconfig.split('\n');
    QStringList lista;
    QStringList interfaces;
    foreach (const QString &linea, lineas) {
        if(linea.isEmpty()){

        }else {
            if(linea[0]!=' ')
                lista.append(linea);
            //qDebug()<<linea[0];
        }

    }

    QStringList palabras;
    QString stemp;
    foreach(const QString &linea, lista){
        palabras=linea.split(" ");
        palabras[0].remove(':');
        interfaces.append(palabras[0]);
    }
    return interfaces;
}
