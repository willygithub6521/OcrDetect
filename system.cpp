#include "system.h"

System* qSlotInstance = 0;
System::System(QObject *parent) :
QObject(parent),
m_imageUrl(""),
m_imagePath("")
{

}

System* System::getInstance()
{
    if (qSlotInstance == 0){
        qSlotInstance = new System();
    }
    return qSlotInstance;
}

QImage System::binarize(const QImage& grayImage, int threshold) {
    QImage result = grayImage;
    for (int y = 0; y < grayImage.height(); ++y) {
        uchar* scanLine = result.scanLine(y);
        for (int x = 0; x < grayImage.width(); ++x) {
            uchar pixel = scanLine[x];
            scanLine[x] = (pixel > threshold) ? 255 : 0;
        }
    }
    return result;
}

QString System::runTesseract(QString imagePath) {
    QProcess process;
    if(imagePath.size() == 0) imagePath = m_imagePath;
    qDebug() << Q_FUNC_INFO << imagePath;
    process.start("tesseract", QStringList() << imagePath << "stdout" << "--psm" << "4");
    process.waitForFinished();
    QString output = process.readAllStandardOutput();
    QString error = process.readAllStandardError();
    qDebug() << Q_FUNC_INFO << output;
    if (!error.isEmpty())
        qDebug() << "Tesseract error:" << error;

    return output;
}

void System::getImageFromClipboard()
{
    const QClipboard *clipboard = QGuiApplication::clipboard();
    const QImage image = clipboard->image();

    if (!image.isNull()) {
//        image.save("clipboard_screenshot.png");
        QString path = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/clipimage.png";
        QImage grayImage = image.convertToFormat(QImage::Format_Grayscale8);
        QImage binarizedImage = binarize(grayImage, 100);
        image.save(path);
        m_imagePath = path;
        m_imageUrl = QUrl::fromLocalFile(path);
        emit imageChanged();
//        binarizedImage.save(path);
//        m_imagePath = path;
        qDebug() << "Saved image from clipboard.";
    } else {
        qDebug() << "No image in clipboard.";
    }
}

void System::saveCsv(const QString& ocrText)
{
    qDebug() << Q_FUNC_INFO << "ocrText: " << ocrText;
    qDebug() << "current path: " << QDir::currentPath();
    QString filePath = QDir::currentPath()+ "/output.csv";
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Can't open file:" << filePath;
        return;
    }

    QTextStream out(&file);

    // 拆成每一行（每一筆資料）
    QStringList lines = ocrText.split(QRegExp("[\r\n]+"), Qt::SkipEmptyParts);

    for (QString line : lines) {
        // 基本清理（可根據實際情況調整）
        line.replace("——-", "");
        line.replace("?", "");
        line.replace("_", "");

        // 如果某些數字沒空格分隔，可再補強處理

        // 用空白分隔欄位 → 改為逗號，符合 CSV 格式
        QStringList tokens = line.split(QRegExp("\\s+"), Qt::SkipEmptyParts);
        QStringList escapedTokens;

        for (const QString& token : tokens) {
            if (token.contains(",")) {
                // 數字含逗號，加上雙引號包住
                escapedTokens << "\"" + token + "\"";
            } else {
                escapedTokens << token;
            }
        }

        QString csvLine = escapedTokens.join(",");

        // 輸出一行
         out << csvLine << "\n";
    }
    file.close();
}

QUrl System::imageUrl() const {
    return m_imageUrl;
}
