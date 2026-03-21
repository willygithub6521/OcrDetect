#include "system.h"
#include <QDateTime>

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
    // 將 psm 從 4 改為 6 (Assume a single uniform block of text)，對這種對齊整齊但缺乏分隔線的純數值表格，辨識率與防錯位會更好。
    process.start("tesseract", QStringList() << imagePath << "stdout" << "--psm" << "6");
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
        if (!m_imagePath.isEmpty()) {
            QFile::remove(m_imagePath);
        }
        QString path = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/clipimage_" + QString::number(QDateTime::currentMSecsSinceEpoch()) + ".png";
        
        // 1. 放大圖片 (Upscaling)：Tesseract 針對大於 20px 特徵的字體辨識度最高。表格數字通常較小，放大 2.5 ~ 3 倍能顯著提升對小數點、逗號與負號的精準度
        QImage scaledImage = image.scaled(image.width() * 3, image.height() * 3, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        
        // 2. 轉灰階 (Grayscale)
        QImage grayImage = scaledImage.convertToFormat(QImage::Format_Grayscale8);
        
        // 3. Tesseract 內建極佳的 Otsu 二值化演算法。若是單純的黑字白(灰)底表格圖片，直接餵入放大後的平滑灰階圖效果通常比手動寫的 binarize(100) 要好，因為 100 容易讓灰色的反鋸齒邊緣變成白底，導致文字斷裂。
        // （註：你原本存檔是存 image.save(path) 導致影像處理都沒生效，這裡改存處理過後的影像）
        grayImage.save(path);
        
        m_imagePath = path;
        m_imageUrl = QUrl::fromLocalFile(path);
        emit imageChanged();
        qDebug() << "Saved image from clipboard.";
    } else {
        qDebug() << "No image in clipboard.";
    }
}

void System::clearImage()
{
    if (!m_imagePath.isEmpty()) {
        QFile::remove(m_imagePath);
        m_imagePath.clear();
    }
    m_imageUrl = QUrl("");
    emit imageChanged();
}

bool System::saveCsv(const QString& ocrText, const QUrl& fileUrl)
{
    qDebug() << Q_FUNC_INFO << "ocrText: " << ocrText;
    
    QString filePath = fileUrl.toLocalFile();
    if (filePath.isEmpty()) {
        filePath = fileUrl.toString(); // Fallback in case it's just a raw path
    }
    
    qDebug() << "target path: " << filePath;
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Can't open file:" << filePath;
        return false;
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
    return true;
}

QUrl System::imageUrl() const {
    return m_imageUrl;
}
