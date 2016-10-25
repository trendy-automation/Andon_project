#ifndef QCONVTYPES_H
#define QCONVTYPES_H

#include <QVariant>
#include "boost/any.hpp"
#include "opc/ua/protocol/variant.h"


/*boost::any AllQVariantToAny(const QVariant& v) {
    switch(v.userType()) {
        case QVariant::BitArray:
            return boost::any(v.value<QBitArray>());
//        case QVariant::Bitmap:
//            return boost::any(v.value<QBitmap>());
        case QVariant::Bool:
            return boost::any(v.value<bool>());
//        case QVariant::Brush:
//            return boost::any(v.value<QBrush>());
        case QVariant::ByteArray:
            return boost::any(v.value<QByteArray>());
        case QVariant::Char:
            return boost::any(v.value<QChar>());
        case QVariant::Color:
            return boost::any(v.value<QColor>());
//        case QVariant::Cursor:
//            return boost::any(v.value<QCursor>());
        case QVariant::Date:
            return boost::any(v.value<QDate>());
        case QVariant::DateTime:
            return boost::any(v.value<QDateTime>());
        case QVariant::Double:
            return boost::any(v.value<double>());
//        case QVariant::EasingCurve:
//            return boost::any(v.value<QEasingCurve>());
//        case QVariant::Font:
//            return boost::any(v.value<QFont>());
//        case QVariant::Hash:
//            return boost::any(v.value<QVariantHash>());
//        case QVariant::Icon:
//            return boost::any(v.value<QIcon>());
//        case QVariant::Image:
//            return boost::any(v.value<QImage>());
        case QVariant::Int:
            return boost::any(v.value<int>());
        case QVariant::KeySequence:
            return boost::any(v.value<QKeySequence>());
//        case QVariant::Line:
//            return boost::any(v.value<QLine>());
//        case QVariant::LineF:
//            return boost::any(v.value<QLineF>());
//        case QVariant::List:
//            return boost::any(v.value<QVariantList>());
//        case QVariant::Locale:
//            return boost::any(v.value<QLocale>());
        case QVariant::LongLong:
            return boost::any(v.value<qlonglong>());
//        case QVariant::Map:
//            return boost::any(v.value<QVariantMap>());
//        case QVariant::Matrix:
//            return boost::any(v.value<QMatrix>());
//        case QVariant::Transform:
//            return boost::any(v.value<QTransform>());
//        case QVariant::Matrix4x4:
//            return boost::any(v.value<QMatrix4x4>());
//        case QVariant::Palette:
//            return boost::any(v.value<QPalette>());
//        case QVariant::Pen:
//            return boost::any(v.value<QPen>());
//        case QVariant::Pixmap:
//            return boost::any(v.value<QPixmap>());
//        case QVariant::Point:
//            return boost::any(v.value<QPoint>());
//        case QVariant::PointArray:
//            return boost::any(v.value<QPointArray>());
//        case QVariant::PointF:
//            return boost::any(v.value<QPointF>());
//        case QVariant::Polygon:
//            return boost::any(v.value<QPolygon>());
//        case QVariant::Quaternion:
//            return boost::any(v.value<QQuaternion>());
//        case QVariant::Rect:
//            return boost::any(v.value<QRect>());
//        case QVariant::RectF:
//            return boost::any(v.value<QRectF>());
//        case QVariant::RegExp:
//            return boost::any(v.value<QRegExp>());
//        case QVariant::Region:
//            return boost::any(v.value<QRegion>());
//        case QVariant::Size:
//            return boost::any(v.value<QSize>());
//        case QVariant::SizeF:
//            return boost::any(v.value<QSizeF>());
//        case QVariant::SizePolicy:
//            return boost::any(v.value<QSizePolicy>());
        case QVariant::String:
            return boost::any(v.value<QString>());
        case QVariant::StringList:
            return boost::any(v.value<QStringList>());
//        case QVariant::TextFormat:
//            return boost::any(v.value<QTextFormat>());
//        case QVariant::TextLength:
//            return boost::any(v.value<QTextLength>());
        case QVariant::Time:
            return boost::any(v.value<QTime>());
        case QVariant::UInt:
            return boost::any(v.value<unsigned>());
        case QVariant::ULongLong:
            return boost::any(v.value<qulonglong>());
//        case QVariant::Url:
//            return boost::any(v.value<QUrl>());
//        case QVariant::Vector2D:
//            return boost::any(v.value<QVector2D>());
//        case QVariant::Vector3D:
//            return boost::any(v.value<QVector3D>());
//        case QVariant::Vector4D:
//            return boost::any(v.value<QVector4D>());
        case QVariant::UserType:
        case QVariant::Invalid:
        default:
            throw std::bad_cast(); //or return default constructed boost::any
    }
}
*/

boost::any QVar2Any(const QVariant& v) {
    switch(v.userType()) {
        case QVariant::BitArray:
            return boost::any(v.value<QBitArray>());
        case QVariant::Bool:
            return boost::any(v.value<bool>());
        case QVariant::ByteArray:
            return boost::any(v.value<QByteArray>());
        case QVariant::Char:
            return boost::any(v.value<QChar>());
        case QVariant::Date:
            return boost::any(v.value<QDate>());
        case QVariant::DateTime:
            return boost::any(v.value<QDateTime>());
        case QVariant::Double:
            return boost::any(v.value<double>());
        case QVariant::LongLong:
            return boost::any(v.value<qlonglong>());
        case QVariant::String:
            return boost::any(v.value<QString>());
        case QVariant::StringList:
            return boost::any(v.value<QStringList>());
        case QVariant::Time:
            return boost::any(v.value<QTime>());
        case QVariant::UInt:
            return boost::any(v.value<unsigned>());
        case QVariant::ULongLong:
            return boost::any(v.value<qulonglong>());
        case QVariant::UserType:
        case QVariant::Invalid:
        default:
            qDebug()<<"Bad casting in QCONVTYPES_H of type " << v.typeName();
            throw std::bad_cast(); //or return default constructed boost::any
    }
}


#endif // QCONVTYPES_H
