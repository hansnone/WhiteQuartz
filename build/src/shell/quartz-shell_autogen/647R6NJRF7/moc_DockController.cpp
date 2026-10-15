/****************************************************************************
** Meta object code from reading C++ file 'DockController.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.12.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../src/shell/dock/DockController.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DockController.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.12.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN6Quartz5Shell14DockControllerE_t {};
} // unnamed namespace

template <> constexpr inline auto Quartz::Shell::DockController::qt_create_metaobjectdata<qt_meta_tag_ZN6Quartz5Shell14DockControllerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "Quartz::Shell::DockController",
        "appBouncingRequested",
        "",
        "cmd",
        "calculateGaussianMagnification",
        "itemCenterDistance",
        "sigma",
        "calculateParabolicBounce",
        "progress",
        "bounceHeight",
        "defaultApps",
        "QVariantList",
        "launchApp",
        "requestAppAttention",
        "baseIconSize",
        "maxMagnifiedSize"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'appBouncingRequested'
        QtMocHelpers::SignalData<void(const QString &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Method 'calculateGaussianMagnification'
        QtMocHelpers::MethodData<qreal(qreal, qreal) const>(4, 2, QMC::AccessPublic, QMetaType::QReal, {{
            { QMetaType::QReal, 5 }, { QMetaType::QReal, 6 },
        }}),
        // Method 'calculateGaussianMagnification'
        QtMocHelpers::MethodData<qreal(qreal) const>(4, 2, QMC::AccessPublic | QMC::MethodCloned, QMetaType::QReal, {{
            { QMetaType::QReal, 5 },
        }}),
        // Method 'calculateParabolicBounce'
        QtMocHelpers::MethodData<qreal(qreal, qreal) const>(7, 2, QMC::AccessPublic, QMetaType::QReal, {{
            { QMetaType::QReal, 8 }, { QMetaType::QReal, 9 },
        }}),
        // Method 'calculateParabolicBounce'
        QtMocHelpers::MethodData<qreal(qreal) const>(7, 2, QMC::AccessPublic | QMC::MethodCloned, QMetaType::QReal, {{
            { QMetaType::QReal, 8 },
        }}),
        // Method 'defaultApps'
        QtMocHelpers::MethodData<QVariantList() const>(10, 2, QMC::AccessPublic, 0x80000000 | 11),
        // Method 'launchApp'
        QtMocHelpers::MethodData<void(const QString &)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Method 'requestAppAttention'
        QtMocHelpers::MethodData<void(const QString &)>(13, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'baseIconSize'
        QtMocHelpers::PropertyData<int>(14, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Constant),
        // property 'maxMagnifiedSize'
        QtMocHelpers::PropertyData<int>(15, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Constant),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<DockController, qt_meta_tag_ZN6Quartz5Shell14DockControllerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject Quartz::Shell::DockController::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN6Quartz5Shell14DockControllerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN6Quartz5Shell14DockControllerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN6Quartz5Shell14DockControllerE_t>.metaTypes,
    nullptr
} };

void Quartz::Shell::DockController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<DockController *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->appBouncingRequested((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: { qreal _r = _t->calculateGaussianMagnification((*reinterpret_cast<std::add_pointer_t<qreal>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<qreal>>(_a[2])));
            if (_a[0]) *reinterpret_cast<qreal*>(_a[0]) = std::move(_r); }  break;
        case 2: { qreal _r = _t->calculateGaussianMagnification((*reinterpret_cast<std::add_pointer_t<qreal>>(_a[1])));
            if (_a[0]) *reinterpret_cast<qreal*>(_a[0]) = std::move(_r); }  break;
        case 3: { qreal _r = _t->calculateParabolicBounce((*reinterpret_cast<std::add_pointer_t<qreal>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<qreal>>(_a[2])));
            if (_a[0]) *reinterpret_cast<qreal*>(_a[0]) = std::move(_r); }  break;
        case 4: { qreal _r = _t->calculateParabolicBounce((*reinterpret_cast<std::add_pointer_t<qreal>>(_a[1])));
            if (_a[0]) *reinterpret_cast<qreal*>(_a[0]) = std::move(_r); }  break;
        case 5: { QVariantList _r = _t->defaultApps();
            if (_a[0]) *reinterpret_cast<QVariantList*>(_a[0]) = std::move(_r); }  break;
        case 6: _t->launchApp((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 7: _t->requestAppAttention((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (DockController::*)(const QString & )>(_a, &DockController::appBouncingRequested, 0))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<int*>(_v) = _t->baseIconSize(); break;
        case 1: *reinterpret_cast<int*>(_v) = _t->maxMagnifiedSize(); break;
        default: break;
        }
    }
}

const QMetaObject *Quartz::Shell::DockController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Quartz::Shell::DockController::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN6Quartz5Shell14DockControllerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Quartz::Shell::DockController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 8;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void Quartz::Shell::DockController::appBouncingRequested(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}
QT_WARNING_POP
