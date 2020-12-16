#include "propertydialog.h"
#include <QStackedWidget>
#include <QMetaProperty>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QSpinBox>
#include <QColorDialog>
#include <QFontDialog>
#include <QDateTimeEdit>
#include <QVariant>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QDateTime>
#include "../qgraphicsplotmacros.h"
#include <QHash>
#include <QtDebug>
#include <QSettings>
#include <QApplication>
#include <QIntValidator>
#include <QDoubleValidator>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QScrollArea>
#include <QSplitter>
#include "plotscenewidget.h"
#include "circleitemset.h"
#include "scaleitem.h"

PropertyDialog::PropertyDialog(PlotSceneWidget *sceneWidget) :
    QDialog(sceneWidget)
{

}

PropertyDialog::~PropertyDialog()
{
    /* just save the save properties on config file checkbox checked status */
    QSettings s;
    s.setValue("PROPERTY_DIALOG_SAVE_PROPS_CHECKED", findChild<QCheckBox *>("cbSaveSettings")->isChecked());
}

void PropertyDialog::itemSelectionChanged()
{
    QList<QTreeWidgetItem *> items = findChild<QTreeWidget *>()->selectedItems();
    qDebug() << __FUNCTION__ << "selected items " << items.size();
    if(items.size() == 1)
    {
        QString name = items.first()->text(0);
        qDebug() << "loading page for " << items.first()->text(0);
        QStackedWidget *sw = findChild<QStackedWidget *>();
        for(int i = 0; i < sw->count(); i++)
        {
            if(sw->widget(i)->objectName() == name)
            {
                sw->setCurrentIndex(i);
                break;
            }
        }
    }
}

void PropertyDialog::show()
{
    const QList<int> supportedTypes = QList<int>() << QVariant::Int << QVariant::Double <<
                                                      QVariant::String << QVariant::Color
                                                   << QVariant::Bool << QVariant::Font
                                                   << QVariant::DateTime;

    QDoubleValidator *doubleValidator = new QDoubleValidator(this);
    QIntValidator *intValidator = new QIntValidator(this);
    QVBoxLayout *vboxlo = new QVBoxLayout(this);
    QStackedWidget *stackW = new QStackedWidget(this);
    QTreeWidget *tree = new QTreeWidget(this);
    QSplitter *splitter = new QSplitter(this);
    tree->setHeaderLabel("Configurable Objects");
    connect(tree, SIGNAL(itemSelectionChanged()), this, SLOT(itemSelectionChanged()));
    splitter->addWidget(tree);
    splitter->addWidget(stackW);

    foreach(QString oName, mObjectsMap.keys())
        // for(int oidx = mObjectsMap.size() - 1; oidx >= 0; oidx--)
    {
        int i;
        QObject *object = mObjectsMap.value(oName);
        bool ok;
        int cnt = 0;
        printf("fetching property for obj %p: %s\n", object, qstoc(oName));
        int propertyConfigurationSuperclass = object->property("__propertyConfigurationSuperclass").toInt(&ok);
        if(!ok)
            propertyConfigurationSuperclass = 1;

        QTreeWidgetItem *classItem = new QTreeWidgetItem(tree, QStringList() << mObjectsMap.key(object));
        const QMetaObject *mo = object->metaObject();
        do{
            QString pageName;
            int row = 0, r;
            int column_offset = 0;
            /* create a page widget */
            QWidget *page = new QWidget(this);
            QGridLayout *pageLo = new QGridLayout(page);
            stackW->addWidget(page);

            cnt++;

            if(cnt > 1)
            {
                /* Avoid unused warning */
                /* QTreeWidgetItem *superClassItem = */ new QTreeWidgetItem(classItem, QStringList() << mo->className());
                classItem->setExpanded(true);
                pageName = mo->className();
            }
            else
                pageName = mObjectsMap.key(object);

            page->setObjectName(pageName);
            page->setProperty("associatedObjectName", mObjectsMap.key(object));

            r = 0; /* current layout row */
            for(i = mo->propertyOffset(); i < mo->propertyCount(); i++)
            {
                QMetaProperty mp = mo->property(i);
                if(supportedTypes.contains(mp.type()))
                {
                    QLabel *label = new QLabel(this);
                    label->setText(mp.name());
                    label->setAlignment(Qt::AlignRight);
                    pageLo->addWidget(label, r, 0 + column_offset, 1, 2);

                    /* Apply push button at the end of the row */
                    if(mp.type() != QVariant::Color)
                    {
                        QPushButton *pb = new QPushButton(this);
                        pb->setProperty("propertyName", mp.name());
                        pb->setProperty("type", mp.type());
                        pb->setText("Apply");
                        connect(pb, SIGNAL(clicked()), this, SLOT(propertyChanged()));
                        pageLo->addWidget(pb, r, 3 + column_offset, 1, 1);
                    }

                    if(mp.type() == QVariant::Int || mp.type() == QVariant::Double)
                    {
                        QLineEdit *le = new QLineEdit(this);
                        if(mp.type() == QVariant::Int)
                        {
                            le->setValidator(intValidator);
                            le->setProperty("type", (int)QVariant::Int);
                        }
                        else
                        {
                            le->setValidator(doubleValidator);
                            le->setProperty("type", (double)QVariant::Double);
                        }
                        le->setObjectName(mp.name());
                        le->setProperty("propertyName", mp.name());
                        le->setText(QString::number(object->property(mp.name()).toDouble()));
                        pageLo->addWidget(le, r, 2 + column_offset, 1, 1);
                    }
                    else if(mp.type() == QVariant::DateTime)
                    {
                        QDateTimeEdit *dte = new QDateTimeEdit(this);
                        dte->setObjectName(mp.name());
                        dte->setCalendarPopup(true);
                        dte->setProperty("propertyName", mp.name());
                        dte->setProperty("type", (int) QVariant::DateTime);
                        QDateTime dt = object->property(mp.name()).toDateTime();
                        dte->setDateTime(dt);
                        pageLo->addWidget(dte, r, 2 + column_offset, 1, 1);
                    }
                    else if(mp.type() == QVariant::Font)
                    {
                        QPushButton *fontButton = new QPushButton(this);
                        fontButton->setObjectName(mp.name());
                        fontButton->setProperty("propertyName", mp.name());
                        fontButton->setProperty("type", (int) QVariant::Font);
                        QFont currentF = object->property(mp.name()).value<QFont>();
                        fontButton->setFont(currentF);
                        fontButton->setText(QString("%1 [%2pt]").arg(currentF.family()).arg(currentF.pointSize()));
                        connect(fontButton, SIGNAL(clicked()), this, SLOT(execFontDialog()));
                        pageLo->addWidget(fontButton, r, 2 + column_offset, 1, 1);
                    }
                    else if(mp.type() == QVariant::Bool)
                    {
                        QCheckBox *cb = new QCheckBox(this);
                        cb->setObjectName(mp.name());
                        cb->setProperty("propertyName", mp.name());
                        cb->setProperty("type", (int)QVariant::Bool);
                        cb->setChecked(object->property(mp.name()).toBool());
                        pageLo->addWidget(cb, r, 2 + column_offset, 1, 1);
                    }

                    else if(mp.type() == QVariant::String)
                    {
                        QLineEdit *le = new QLineEdit(this);
                        le->setObjectName(mp.name());
                        le->setProperty("propertyName", mp.name());
                        le->setProperty("type", (int)QVariant::String);
                        le->setText(object->property(mp.name()).toString());
                        pageLo->addWidget(le, r, 2 + column_offset, 1, 1);
                    }

                    else if(mp.type() == QVariant::Color)
                    {
                        QColor c = object->property(mp.name()).value<QColor>();
                        QPushButton *pbcolor = new QPushButton(this);
                        pbcolor->setPalette(QPalette(c));
                        pbcolor->setObjectName(mp.name());
                        pbcolor->setProperty("type", (int)QVariant::Color);
                        pbcolor->setProperty("propertyName", mp.name());
                        pageLo->addWidget(pbcolor, r, 2 + column_offset, 1, 1);
                        connect(pbcolor, SIGNAL(clicked()), this, SLOT(chooseColorButton()));
                        pbcolor->setText("Pick Color...");
                    }
                    else
                        printf("QVariant %d type (%s) unhandled\n", mp.type(), mp.typeName());

                    r++;

                    if(r % 18 == 0)
                    {
                        column_offset = (pageLo->rowCount() / 18 ) * pageLo->columnCount();
                        r = 0; /* restart from row 0 */
                    }
                }
            }/* end for */
            mo = mo->superClass();

            /* dynamic properties! */
            /* cnt == 1 not to add multiple times in class and superclasses */
            if(row == 0 && cnt == 1) /* if not yet loaded */
            {
                QFont f;
                row = r;
                QList<QByteArray> dynamicProperties = object->dynamicPropertyNames();
                if(dynamicProperties.size())
                {
                    QLabel *specialPropLabel = new QLabel(this);
                    specialPropLabel->setText("Custom configuration properties");
                    f = specialPropLabel->font();
                    f.setBold(true);
                    specialPropLabel->setFont(f);
                    pageLo->addWidget(specialPropLabel, row, 0 + column_offset, 1, 3);
                    specialPropLabel->setAlignment(Qt::AlignHCenter);
                    specialPropLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
                    row++;
                }
                /* dynamic properties are supposed to be application specific
                 * and so they are marked in bold
                 */
                foreach(QByteArray prop, dynamicProperties)
                {
                    QString propertyName = QString(prop);
                    if(!propertyName.startsWith("__"))
                    {
                        QLabel *label = new QLabel(this);
                        label->setFont(f);
                        label->setText(propertyName);
                        label->setAlignment(Qt::AlignRight);
                        pageLo->addWidget(label, row, 0 + column_offset, 1, 2);

                        QLineEdit *le = new QLineEdit(this);
                        le->setObjectName(prop);
                        le->setProperty("propertyName",prop);
                        le->setProperty("type", (int) object->property(qstoc(propertyName)).type());
                        le->setText(object->property(prop).toString());
                        pageLo->addWidget(le, row, 2 + column_offset, 1, 1);

                        QPushButton *pb = new QPushButton(this);
                        pb->setProperty("propertyName", prop);
                        pb->setProperty("type", (int) object->property(qstoc(propertyName)).type());
                        pb->setText("Apply");
                        connect(pb, SIGNAL(clicked()), this, SLOT(propertyChanged()));
                        pageLo->addWidget(pb, row, 3 + column_offset, 1, 1);
                        row++;
                    }
                }
            }

        }while(cnt < propertyConfigurationSuperclass);

        /// not working yet...

        //        QPushButton *pbRemoveKeys = new QPushButton(this);
        //        pbRemoveKeys->setText("Clear Settings");
        //        pbRemoveKeys->setProperty("associatedSettingsKey", mObjectsMap.key(object));
        //        pbRemoveKeys->setToolTip("Clears the previously saved settings.\nThe next time you run the "
        //                                 "application,\nthe default settings will be loaded.");
        //        connect(pbRemoveKeys, SIGNAL(clicked()),this, SLOT(clearSettings()));
        //        printf("adding pb on row %d - obj %s\n", i, qstoc(mObjectsMap.key(object)));
        //        pageLo->addWidget(pbRemoveKeys, i, 2, 1, 1)

    }

    QCheckBox *cbSaveSettings = new QCheckBox(this);
    cbSaveSettings->setText("Remember settings for " + this->mSettingsKey);
    cbSaveSettings->setObjectName("cbSaveSettings");
    QSettings s;
    cbSaveSettings->setChecked(s.value("PROPERTY_DIALOG_SAVE_PROPS_CHECKED", true).toBool());

    /* add widgets to vbox layout */
    vboxlo->addWidget(splitter);
    vboxlo->addWidget(cbSaveSettings);

    QList<QTreeWidgetItem *> items = tree->findItems("PlotSceneWidget", Qt::MatchWildcard|Qt::MatchRecursive);
    if(items.size())
    {
        items.first()->setSelected(true);
        itemSelectionChanged();
    }

    return QDialog::show();

}

void PropertyDialog::clearSettings()
{
    QObject *oSender = sender();
    QString settingsKey = oSender->property("associatedSettingsKey").toString();
    QSettings s;
    printf("clearing settings for \"%s\"\n", qstoc(settingsKey));
    s.remove(mSettingsKey + "/" + settingsKey);
}

void PropertyDialog::propertyChanged()
{
    QSettings s;
    QObject *sen = sender();
    QString propName = sen->property("propertyName").toString();
    QStackedWidget *stackw = findChild<QStackedWidget *>();
    QWidget *currentWidget = stackw->currentWidget();
    QWidget *w = currentWidget->findChild<QWidget*> (propName);
    //  printf("property type %d widget type %s\n", w->property("type").toInt(), w->metaObject()->className());

    bool saveSettings = findChild<QCheckBox *>("cbSaveSettings")->isChecked();

    QObject *targetObject = mObjectsMap.value(currentWidget->property("associatedObjectName").toString());

    qDebug() << __FUNCTION__ << "targetObject is " << targetObject->objectName() << "was looking for "
             << currentWidget->property("associatedObjectName").toString();

    qDebug() << __FUNCTION__ << "widget looking for is " << w << "sender property name is " << propName;

    if(w)
    {
        QString settingsKey;
        bool success = false;
        if(saveSettings)
            settingsKey = mGetQSettingsKey(targetObject, propName);

        if(settingsKey.isEmpty() && saveSettings)
        {
            perr("PropertyDialog::propertyChanged: no settings key provided for application \"%s\"",
                 qstoc(qApp->applicationName()));
        }
        else if(!propName.isEmpty() && !targetObject->objectName().isEmpty())
        {
            qDebug() << __FUNCTION__ << w->property("type").toInt();
            switch(w->property("type").toInt())
            {
            case QVariant::Int:
                success = targetObject->setProperty(qstoc(propName), qobject_cast<QLineEdit *>(w)->text().toInt());
                if(saveSettings)
                    s.setValue(settingsKey, qobject_cast<QLineEdit *>(w)->text().toInt());
                break;
            case QVariant::Double:
                success = targetObject->setProperty(qstoc(propName), qobject_cast<QLineEdit *>(w)->text().toDouble());
                if(saveSettings)
                    s.setValue(settingsKey, qobject_cast<QLineEdit *>(w)->text().toDouble());
                break;
            case QVariant::String:
                success = targetObject->setProperty(qstoc(propName), qobject_cast<QLineEdit *>(w)->text());
                if(saveSettings)
                    s.setValue(settingsKey, qobject_cast<QLineEdit *>(w)->text());
                break;
            case QVariant::Bool:
                success = targetObject->setProperty(qstoc(propName), qobject_cast<QCheckBox *>(w)->isChecked());
                if(saveSettings)
                    s.setValue(settingsKey, qobject_cast<QCheckBox *>(w)->isChecked());
                break;
            case QVariant::DateTime:
                printf("save: date time\n");
                success = targetObject->setProperty(qstoc(propName), qobject_cast<QDateTimeEdit *>(w)->dateTime());
                if(saveSettings)
                    s.setValue(settingsKey, qobject_cast<QDateTimeEdit *>(w)->dateTime());
                break;
            }
            /* if the property is a dynamic one, then setProperty never returns true. Do not report an error
             * in that case.
             */
            if(!success && !targetObject->dynamicPropertyNames().contains(QByteArray(qstoc(propName))))
                perr("PropertyDialog::propertyChanged: failed to set property \"%s\" on object \"%s\"",
                     qstoc(propName), qstoc(targetObject->objectName()));
        }
        if(propName.isEmpty())
            perr("PropertyDialog::propertyChanged: property name is empty");
        if(targetObject->objectName().isEmpty())
            perr("PropertyDialog::propertyChanged: object name empty for an object of class \"%s\"",
                 targetObject->metaObject()->className());

    }
}

void PropertyDialog::chooseColorButton()
{
    QSettings s;
    QPushButton *pb = qobject_cast<QPushButton *>(sender());
    QString propName = pb->property("propertyName").toString();
    QColor c = pb->palette().color(QPalette::Button);
    QColor col = QColorDialog::getColor(c);

    QStackedWidget *stackw = findChild<QStackedWidget *>();
    QWidget *currentWidget = stackw->currentWidget();
    QObject *targetObject = mObjectsMap.value(currentWidget->property("associatedObjectName").toString());

    if(col.isValid())
    {
        bool saveSettings = findChild<QCheckBox *>("cbSaveSettings")->isChecked();

        //  qDebug() << "Color " << col << targetObject << propName;
        targetObject->setProperty(qstoc(propName), col);
        if(saveSettings)
        {
            QString settingsKey = mGetQSettingsKey(targetObject, propName);
            s.setValue(settingsKey, col);
        }
        pb->setPalette(col);
    }
}

void PropertyDialog::execFontDialog()
{
    bool ok;
    QSettings s;
    QPushButton *pb = qobject_cast<QPushButton *>(sender());
    QString propName = pb->property("propertyName").toString();
    QFont f = pb->font();
    QFont newFont = QFontDialog::getFont(&ok, f, this);
    if(ok)
    {
        QStackedWidget *stackw = findChild<QStackedWidget *>();
        QObject *targetObject = mObjectsMap.value(stackw->currentWidget()->property("associatedObjectName").toString());
        bool saveSettings = findChild<QCheckBox *>("cbSaveSettings")->isChecked();
        targetObject->setProperty(qstoc(propName), newFont);
        if(saveSettings)
        {
            QString settingsKey = mGetQSettingsKey(targetObject, propName);
            s.setValue(settingsKey, newFont);
        }
        pb->setFont(newFont);
        pb->setText(QString("%1 [%2pt]").arg(newFont.family()).arg(newFont.pointSize()));
    }
}

QString PropertyDialog::mGetQSettingsKey(QObject* o, const QString& propName) const
{
    QString ret;
    ret = mSettingsKey + "/" + o->objectName() + "/" + propName;
    return ret;
}


