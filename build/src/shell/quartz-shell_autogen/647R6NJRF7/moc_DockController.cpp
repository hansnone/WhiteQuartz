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
        "appsChanged",
        "trashIconChanged",
        "icon",
        "onWindowAdded",
        "appId",
        "title",
        "active",
        "onWindowRemoved",
        "onActiveWindowChanged",
        "calculateGaussianMagnification",
        "itemCenterDistance",
        "sigma",
        "calculateParabolicBounce",
        "progress",
        "bounceHeight",
        "defaultApps",
        "QVariantList",
        "parseCommandLine",
        "launchApp",
        "requestAppAttention",
        "listStackDirectory",
        "dirPath",
        "updateTrashState",
        "quitApp",
        "forceQuitApp",
        "toggleKeepInDock",
        "baseIconSize",
        "maxMagnifiedSize",
        "apps",
        "trashIcon"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'appBouncingRequested'
        QtMocHelpers::SignalData<void(const QString &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Signal 'appsChanged'
        QtMocHelpers::SignalData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'trashIconChanged'
        QtMocHelpers::SignalData<void(const QString &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 6 },
        }}),
        // Slot 'onWindowAdded'
        QtMocHelpers::SlotData<void(const QString &, const QString &, const QString &, bool)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 8 }, { QMetaType::QString, 9 }, { QMetaType::QString, 6 }, { QMetaType::Bool, 10 },
        }}),
        // Slot 'onWindowRemoved'
        QtMocHelpers::SlotData<void(const QString &)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 8 },
        }}),
        // Slot 'onActiveWindowChanged'
        QtMocHelpers::SlotData<void(const QString &, const QString &)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 8 }, { QMetaType::QString, 9 },
        }}),
        // Method 'calculateGaussianMagnification'
        QtMocHelpers::MethodData<qreal(qreal, qreal) const>(13, 2, QMC::AccessPublic, QMetaType::QReal, {{
            { QMetaType::QReal, 14 }, { QMetaType::QReal, 15 },
        }}),
        // Method 'calculateGaussianMagnification'
        QtMocHelpers::MethodData<qreal(qreal) const>(13, 2, QMC::AccessPublic | QMC::MethodCloned, QMetaType::QReal, {{
            { QMetaType::QReal, 14 },
        }}),
        // Method 'calculateParabolicBounce'
        QtMocHelpers::MethodData<qreal(qreal, qreal) const>(16, 2, QMC::AccessPublic, QMetaType::QReal, {{
            { QMetaType::QReal, 17 }, { QMetaType::QReal, 18 },
        }}),
        // Method 'calculateParabolicBounce'
        QtMocHelpers::MethodData<qreal(qreal) const>(16, 2, QMC::AccessPublic | QMC::MethodCloned, QMetaType::QReal, {{
            { QMetaType::QReal, 17 },
        }}),
        // Method 'defaultApps'
        QtMocHelpers::MethodData<QVariantList() const>(19, 2, QMC::AccessPublic, 0x80000000 | 20),
        // Method 'parseCommandLine'
        QtMocHelpers::MethodData<QStringList(const QString &) const>(21, 2, QMC::AccessPublic, QMetaType::QStringList, {{
            { QMetaType::QString, 3 },
        }}),
        // Method 'launchApp'
        QtMocHelpers::MethodData<void(const QString &)>(22, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Method 'requestAppAttention'
        QtMocHelpers::MethodData<void(const QString &)>(23, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Method 'listStackDirectory'
        QtMocHelpers::MethodData<QVariantList(const QString &) const>(24, 2, QMC::AccessPublic, 0x80000000 | 20, {{
            { QMetaType::QString, 25 },
        }}),
        // Method 'updateTrashState'
        QtMocHelpers::MethodData<void()>(26, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'quitApp'
        QtMocHelpers::MethodData<void(const QString &)>(27, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Method 'forceQuitApp'
        QtMocHelpers::MethodData<void(const QString &)>(28, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Method 'toggleKeepInDock'
        QtMocHelpers::MethodData<void(const QString &)>(29, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'baseIconSize'
        QtMocHelpers::PropertyData<int>(30, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Constant),
        // property 'maxMagnifiedSize'
        QtMocHelpers::PropertyData<int>(31, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Constant),
        // property 'apps'
        QtMocHelpers::PropertyData<QVariantList>(32, 0x80000000 | 20, QMC::DefaultPropertyFlags | QMC::EnumOrFlag, 1),
        // property 'trashIcon'
        QtMocHelpers::PropertyData<QString>(33, QMetaType::QString, QMC::DefaultPropertyFlags, 2),
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
        case 1: _t->appsChanged(); break;
        case 2: _t->trashIconChanged((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->onWindowAdded((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[4]))); break;
        case 4: _t->onWindowRemoved((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 5: _t->onActiveWindowChanged((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 6: { qreal _r = _t->calculateGaussianMagnification((*reinterpret_cast<std::add_pointer_t<qreal>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<qreal>>(_a[2])));
            if (_a[0]) *reinterpret_cast<qreal*>(_a[0]) = std::move(_r); }  break;
        case 7: { qreal _r = _t->calculateGaussianMagnification((*reinterpret_cast<std::add_pointer_t<qreal>>(_a[1])));
            if (_a[0]) *reinterpret_cast<qreal*>(_a[0]) = std::move(_r); }  break;
        case 8: { qreal _r = _t->calculateParabolicBounce((*reinterpret_cast<std::add_pointer_t<qreal>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<qreal>>(_a[2])));
            if (_a[0]) *reinterpret_cast<qreal*>(_a[0]) = std::move(_r); }  break;
        case 9: { qreal _r = _t->calculateParabolicBounce((*reinterpret_cast<std::add_pointer_t<qreal>>(_a[1])));
            if (_a[0]) *reinterpret_cast<qreal*>(_a[0]) = std::move(_r); }  break;
        case 10: { QVariantList _r = _t->defaultApps();
            if (_a[0]) *reinterpret_cast<QVariantList*>(_a[0]) = std::move(_r); }  break;
        case 11: { QStringList _r = _t->parseCommandLine((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QStringList*>(_a[0]) = std::move(_r); }  break;
        case 12: _t->launchApp((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 13: _t->requestAppAttention((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 14: { QVariantList _r = _t->listStackDirectory((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QVariantList*>(_a[0]) = std::move(_r); }  break;
        case 15: _t->updateTrashState(); break;
        case 16: _t->quitApp((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 17: _t->forceQuitApp((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 18: _t->toggleKeepInDock((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (DockController::*)(const QString & )>(_a, &DockController::appBouncingRequested, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (DockController::*)()>(_a, &DockController::appsChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (DockController::*)(const QString & )>(_a, &DockController::trashIconChanged, 2))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<int*>(_v) = _t->baseIconSize(); break;
        case 1: *reinterpret_cast<int*>(_v) = _t->maxMagnifiedSize(); break;
        case 2: *reinterpret_cast<QVariantList*>(_v) = _t->apps(); break;
        case 3: *reinterpret_cast<QString*>(_v) = _t->trashIcon(); break;
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
        if (_id < 19)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 19;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 19)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 19;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void Quartz::Shell::DockController::appBouncingRequested(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void Quartz::Shell::DockController::appsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void Quartz::Shell::DockController::trashIconChanged(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}
QT_WARNING_POP
