#ifndef SYSTEM_H
#define SYSTEM_H
#include <QObject>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QDir>
#include <QCryptographicHash>
#include <QThread>
#include <QList>
#include <QNetworkInterface>
#include <QHostAddress>

#include <QStandardPaths>
#include <QClipboard>
#include <QGuiApplication>
#include <QImage>
#include <QPixmap>
#include <QUrl>
#include <QProcess>

class System: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl imageUrl READ imageUrl NOTIFY imageChanged)
public:
    System(QObject *parent = nullptr);
    static System* getInstance();
    QImage binarize(const QImage& grayImage, int threshold = 128);
    Q_INVOKABLE QString runTesseract(QString imagePath = "");
    Q_INVOKABLE void getImageFromClipboard();
    Q_INVOKABLE void saveCsv(const QString& ocrText);

    QUrl imageUrl() const;
public slots:
signals:
    void imageChanged();
private:
    QUrl m_imageUrl;
    QString m_imagePath;
};

#endif // SYSTEM_H
