/***************************************************************************
 *   Copyright (C) 2008 by Giovanni Romano                                 *
 *   Giovanni.Romano.76@gmail.com                                          *
 *                                                                         *
 *   This program may be distributed under the terms of the Q Public       *
 *   License as defined by Trolltech AS of Norway and appearing in the     *
 *   file LICENSE.QPL included in the packaging of this file.              *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                  *
 ***************************************************************************/


#include "widgetKeyBoard.h"
#include <QGuiApplication>
#include <QLayout>
#include <QScreen>
#include <QKeyEvent>
#include <QDir>
#include <qDebug>
#include <QAudio>
//#include <QTemporaryFile>
#include <QMultimedia>
#include <QtMultimedia>


bool widgetKeyBoard::m_created = false;

widgetKeyBoard::widgetKeyBoard(bool embeddedKeyboard, QWidget *activeForm, QWidget *parent) : QWidget(parent), m_nextInput(NULL), m_activeWindow(activeForm),
        m_currentTextBox(NULL), m_embeddedKeyboard(embeddedKeyboard), m_echoMode(false), m_zoomFacilityEmbedded(false), m_enablePasswordEcho(false),
        m_clipboard(QGuiApplication::clipboard())
      //,m_player("")
{        

    const QString outputFile = QDir::tempPath() + "/click.wav";
    QFile::copy (CLICK_SOUND, outputFile);
    //m_player(outputFile);
    //QSound::play(outputFile);
    //QSound::play(QDir::tempPath() + "/click.wav");
    //QFile::remove(outputFile);


   /* qDebug() << "m_sndfile.open: " <<m_sndfile.open(QIODevice::ReadOnly);
    QAudioFormat format;
//      format.setFrequency(8000);
//      format.setChannels(1);
      format.setSampleSize(8);
      format.setCodec("audio/pcm");
      format.setByteOrder(QAudioFormat::LittleEndian);
      format.setSampleType(QAudioFormat::UnSignedInt);
      foreach(const QAudioDeviceInfo &deviceInfo, QAudioDeviceInfo::availableDevices(QAudio::AudioOutput))
      qDebug() << "Device name: " << deviceInfo.deviceName();
*/
      //m_audio = new QAudioOutput;//( format, 0);

    this->m_clipboard->clear();
    this->setWindowIcon(QPixmap(":/TastieraVirtuale/images/logo"));

QString lang = ":/Translator/translations/virtualBoard_"+ QLocale::system().name().mid(0, 2);
if (qtTranslator.load(lang)){
    QGuiApplication::installTranslator(&qtTranslator);
    qtTranslator.setProperty("lang",lang);
}
else  qDebug()<<"Failed to load a translation for QT in your local language:"
        <<QLocale::system().name().mid(0, 2)<<"from"<< lang;
}

widgetKeyBoard::~widgetKeyBoard()
{
}


void widgetKeyBoard::enableSwitchingEcho(bool status)
{
    this->m_enablePasswordEcho = status;
}

bool widgetKeyBoard::isEnabledSwitchingEcho(void)
{
    return (this->m_enablePasswordEcho);
}

/*inline */ bool widgetKeyBoard::isEmbeddedKeyboard(void)
{
    return (this->m_embeddedKeyboard);
}

void widgetKeyBoard::setZoomFacility(bool active)
{
    this->m_zoomFacilityEmbedded = active;
}

bool widgetKeyBoard::isZoomFacilityEnable(void)
{
    return (this->m_zoomFacilityEmbedded);
}

QKeyPushButton * widgetKeyBoard::createNewKey(QString keyValue)
{
    QKeyPushButton *tmp = new QKeyPushButton(this);
    QString        style = QString(DEFAULT_STYLE_BUTTON) + QString(DEFAULT_BACKGROUND_BUTTON);
    QSize          imageDim;
    int            width = 0, height = 0;

    if (this->isEmbeddedKeyboard() == true) {
        imageDim.setWidth(16);
        imageDim.setHeight(16);
    }
    else {
        imageDim.setWidth(32);
        imageDim.setHeight(32);
    }
    if (IS_PASSWORD(keyValue) == true || IS_PASSWORD_EMB(keyValue) == true) {
        tmp->setIconSize(imageDim);
        tmp->setIcon(QPixmap(":/TastieraVirtuale/normalEcho"));
    }
    else if (IS_PASTE(keyValue) == true) {
        tmp->setIconSize(imageDim);
        tmp->setEnabled(false);
        tmp->setIcon(QPixmap(":/TastieraVirtuale/paste"));
    }
    else if (IS_COPY(keyValue) == true) {
        tmp->setIconSize(imageDim);
        tmp->setIcon(QPixmap(":/TastieraVirtuale/copy"));
    }
    else if (IS_CUT_LEFT(keyValue) == true) {
        tmp->setIconSize(imageDim);
        tmp->setIcon(QPixmap(":/TastieraVirtuale/cut"));
    }
    tmp->setText(keyValue);
    if (this->isEmbeddedKeyboard() == true) {
        width = KEY_WIDTH_EMBEDDED;
        height = KEY_HEIGHT_EMBEDDED;
        style += QString(EMBEDDED_KEYBOARD);
    }
    else {
        width = 54;
        height = 40;
    }
    tmp->setObjectName(keyValue);
    tmp->setMinimumSize(width, height);
    tmp->setMaximumSize(width, height);
    tmp->setStyleSheet(style);
    tmp->setVisible(true);
    return (tmp);
}

void widgetKeyBoard::controlKeyEcho(QLineEdit *control)
{
    QString         textToSearch;
    QKeyPushButton  *echoControlKey;
    QString         tmpStyle;
    QPushButton     *pasteButton = this->findChild<QPushButton *>(KEY_PASTE);
    QPushButton     *cutButton = this->findChild<QPushButton *>(KEY_CUT_LEFT);
    QPushButton     *copyButton = this->findChild<QPushButton *>(KEY_COPY);

    if (this->isEmbeddedKeyboard()) {
        textToSearch = KEY_HIDECHAR_EMBEDDED;
        tmpStyle = QString(EMBEDDED_KEYBOARD);
    }
    else
        textToSearch = KEY_HIDECHAR;
    if (pasteButton != NULL && copyButton != NULL) { // support for copy/paste functionality
        if (control->echoMode() == QLineEdit::Normal) {
            cutButton->setEnabled(true);
            copyButton->setEnabled(true);
            if (this->m_clipboard->text().length() > 0)
                pasteButton->setEnabled(true);
            else
                pasteButton->setEnabled(false);
        }
        else if (control->echoMode() == QLineEdit::Password) {
            copyButton->setEnabled(false);
            cutButton->setEnabled(false);
            pasteButton->setEnabled(false);
        }
    }
    echoControlKey = this->findChild<QKeyPushButton *>(textToSearch);
    if (echoControlKey != NULL) { // support for password echo functionality            
        if (control->echoMode() == QLineEdit::Normal) {            
            echoControlKey->setStyleSheet(QString(DEFAULT_STYLE_BUTTON) + QString(DEFAULT_BACKGROUND_BUTTON) +  tmpStyle); // segnalato come attivo
            echoControlKey->setIcon(QPixmap(":/TastieraVirtuale/normalEcho"));
        }
        else if (control->echoMode() == QLineEdit::Password) {            
            echoControlKey->setStyleSheet(QString(DEFAULT_STYLE_BUTTON) + QString(CHANGED_BACKGROUND_BUTTON) + tmpStyle); // segnalato come attivo
            echoControlKey->setIcon(QPixmap(":/TastieraVirtuale/passwdEcho"));
        }
        echoControlKey->repaint();
        QGuiApplication::processEvents();
    }
}

//
// riceve i caratteri che sono stati digitati:
void widgetKeyBoard::receiptChildKey(QKeyEvent *event, QLineEdit *focusThisControl, bool reset)
{
    static QLineEdit    *nextInput = NULL;

    if (reset == true) { // reinizializza il controllo
        nextInput = this->getNextTextbox(focusThisControl, true);
        return;
}
    if (nextInput == NULL)
        return;
//
    // inizia l'analisi del carattere ricevuto:
    QString newKey = event->text();
    QString tmpReceiptString = nextInput->text();
    int     tmpPos = nextInput->cursorPosition();

    if (NO_SPECIAL_KEY(newKey) == false) {
        if (IS_RETURN(newKey) == true || IS_TAB(newKey) == true) { // trattasi di TAB, si sposta alla text successiva
            nextInput = this->setDefaultTextStyle(nextInput);
            nextInput->deselect();
            nextInput = this->getNextTextbox();
            this->controlKeyEcho(nextInput); // status of key echo here
            if (nextInput != NULL) {
                nextInput->setCursorPosition(nextInput->text().length()); // comment this line if you want caret position at current char inserted
                nextInput->setFocus(Qt::TabFocusReason);
            }
        }
        else if (IS_BACK(newKey) == true || IS_BACK_EMB(newKey) == true) { // trattasi di CANCELLARE carattere, elimina il carattere a sinistra
            if (tmpPos-1 >= 0) {
                tmpReceiptString = tmpReceiptString.remove(tmpPos-1, 1);
                nextInput->setText(tmpReceiptString);
                nextInput->setCursorPosition(tmpPos-1);
                nextInput->setSelection(tmpPos-2, 1);
            }
        }
        else if (IS_CANC(newKey) == true) { // trattasi di CANC carattere, elimina il carattere a destra
            //CANC text en - ru
            //TODO move qtTranslator to mainwindow or app
            QGuiApplication::removeTranslator(&qtTranslator);
            QString lang;
        if (qtTranslator.property("lang").toString()==":/Translator/translations/virtualBoard_en")
            lang = ":/Translator/translations/virtualBoard_" + QLocale::system().name().mid(0, 2);
        else lang = ":/Translator/translations/virtualBoard_en";
            qtTranslator.setProperty("lang",lang);
        if (qtTranslator.load(lang))
            QGuiApplication::installTranslator(&qtTranslator);
        else  qDebug()<<"Failed to load a translation for QT in your local language:"
                <<"from"<<lang;
//        widgetKeyBoard::m_created = false;
//        createKeyboard();
        recreateKeyboard();

//             tmpReceiptString = tmpReceiptString.remove(tmpPos, 1);
//             nextInput->setText(tmpReceiptString);
//             nextInput->setCursorPosition(tmpPos);
//             nextInput->setSelection(tmpPos-1, 1);
        }
        else if (IS_COPY(newKey) == true || IS_CUT_LEFT(newKey) == true) {
            QPushButton *button = this->findChild<QPushButton *>(KEY_PASTE);

            if (button != NULL) {
                if (nextInput->text().length() != 0) {
                    this->m_clipboard->setText(nextInput->text());
                    if (IS_CUT_LEFT(newKey) == true)
                        nextInput->setText("");
                    button->setEnabled(true);
                }
                else
                    button->setEnabled(false);
            }
        }
        else if (IS_PASTE(newKey) == true)
            nextInput->setText(this->m_clipboard->text());
        else if (IS_ALT(newKey) == true || IS_CTRL_LEFT(newKey) == true)
        {
            ; // non esegue nessuna operazione
        }
    }
    else { // si tratta di un carattere da visualizzare nella casella di testo corrente
        tmpReceiptString = tmpReceiptString.insert(tmpPos, newKey);
        nextInput->setText(tmpReceiptString);
        nextInput->setCursorPosition(tmpPos+1);
        nextInput->setSelection(tmpPos, 1);
    }
    if (IS_RETURN(newKey)) emit returnPressed();
}


void widgetKeyBoard::switchKeyEchoMode(QLineEdit *control)
{
    this->setStatusEchoMode(!this->statusEchoMode());
    if (this->statusEchoMode() == true)
        control->setEchoMode(QLineEdit::Password);
    else
        control->setEchoMode(QLineEdit::Normal);
    this->controlKeyEcho(control);
}


QLineEdit * widgetKeyBoard::getNextTextbox(QLineEdit *thisControl, bool reset)
{
    QLineEdit	*tmpInputFound = NULL;

    this->m_currentTextBox = NULL;
    if (this->m_activeWindow == NULL) { // nessuna finestra principale su cui operare
        m_nextInput = NULL;
        return (NULL);
    }
    if (thisControl != NULL) {
        if (this->m_activeWindow->findChild<QLineEdit *>(thisControl->objectName()) == 0) // verifies that thisControl is a child of m_activeWindow
            return (NULL);
        else // it's true!
            return (this->setCurrentTextStyle(thisControl));
    }
    //
    // cicla nella catena dei controlli in ordine di focus per ottenere la QEditLine
    if (m_nextInput == NULL || reset == true)
        m_nextInput = this->m_activeWindow->nextInFocusChain();
    else
        m_nextInput = m_nextInput->nextInFocusChain();
    do {
        if (m_nextInput->inherits("QLineEdit") == true) { // trovata una casella di testo da utilizzare
            tmpInputFound = dynamic_cast<QLineEdit *>(m_nextInput);
            return (this->setCurrentTextStyle(tmpInputFound));
        }
        m_nextInput = m_nextInput->nextInFocusChain();
    } while (m_nextInput != NULL);
    return (NULL);
}


QLineEdit * widgetKeyBoard::setCurrentTextStyle(QLineEdit *control)
{
    this->m_currentTextBox = control;
    control->setStyleSheet(STYLE_FOCUS_TEXT);
    return (control);
}

QLineEdit * widgetKeyBoard::setDefaultTextStyle(QLineEdit *control)
{
    control->setStyleSheet(this->m_noFocusPalette.styleSheet());
    return (control);
}

void widgetKeyBoard::init_keyboard(QLineEdit *focusThisControl)
{
    this->m_currentTextBox = NULL;
    this->m_nextInput = focusThisControl;
    this->receiptChildKey(NULL, focusThisControl, true);
}

void widgetKeyBoard::soundClick(void)
{
    //TODO make good aprouch
    //m_audio->start(&m_sndfile);
    QSound::play(QDir::tempPath() + "/click.wav");
}

void widgetKeyBoard::show(QWidget *activeForm)
{
    this->m_activeWindow = activeForm;
    this->init_keyboard(NULL);
    if (this->windowState() == Qt::WindowMinimized)
        this->setWindowState(Qt::WindowNoState);
    this->setStatusEchoMode(false);
    this->m_clipboard->clear();
    this->move(20, QGuiApplication::screens()[0]->availableGeometry().height() - this->height() - 200);
    //this->borderFrame(true);
    QWidget::show();
}


void widgetKeyBoard::borderFrame(bool visible)
{
    if (visible == true)
        this->setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint | Qt::MSWindowsFixedSizeDialogHint);
    else
        this->setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint | Qt::MSWindowsFixedSizeDialogHint | Qt::CustomizeWindowHint);
}


void widgetKeyBoard::focusThis(QLineEdit *control)
{
    if (this->m_activeWindow != NULL) {
        if (this->m_currentTextBox != NULL)
            this->m_currentTextBox = this->setDefaultTextStyle(this->m_currentTextBox);
        this->init_keyboard(control);
        this->controlKeyEcho(control);
    }
    else {
        ;// Non è attiva nessuna finestra su cui operare
    }
}

void widgetKeyBoard::closeEvent(QCloseEvent * event)
{
    emit returnPressed();
    //event->ignore();
}


void widgetKeyBoard::hide(bool changeColor)
{
    try {
        if (changeColor == true)
            if (this->m_activeWindow != NULL && this->m_currentTextBox != NULL)
                this->m_currentTextBox = this->setDefaultTextStyle(this->m_currentTextBox);
        this->m_currentTextBox = NULL;
        this->m_activeWindow = NULL;
        setVisible(false);
    }
    catch (...) {
    }
}


void widgetKeyBoard::createKeyboard(void)
{
	QKeyPushButton	*tmp = NULL;	
    QVBoxLayout     *tmpVLayout = new QVBoxLayout;
    QHBoxLayout     *tmpLayout = new QHBoxLayout;
    QString         tmpStyle = QString::null;

    if (widgetKeyBoard::m_created == true) // tastiera già  creata: esce
		return;
    widgetKeyBoard::m_created = true; // segnala che è stata creata la tastiera
	//
    int spacing = 2;
    tmpVLayout->setSpacing(spacing);
    tmpLayout->setSpacing(spacing);
    // stampa tasti numerici:
	tmpLayout->addWidget(createNewKey("\\"));
	for (short i = 49; i <= 57; i++) {
		tmpLayout->addWidget(createNewKey(QChar(i)));
	}
    tmpLayout->addWidget(createNewKey(tr("0")));
    tmpLayout->addWidget(createNewKey(tr("/")));
    tmpLayout->addWidget(createNewKey(tr("?")));
    tmpLayout->addWidget(createNewKey(tr("'")));
    if (this->isEmbeddedKeyboard() == true)
        tmpStyle = QString(KEY_BACKSPACE_EMBEDDED);
    else
        tmpStyle = QString(KEY_BACKSPACE);
    tmp = createNewKey(tmpStyle);
    tmp->setMaximumWidth(tmp->maximumWidth() * 2);
    tmp->setMinimumWidth(tmp->minimumWidth() * 2);
	tmpLayout->addWidget(tmp);
    //tmpStyle = QString(KEY_CANC);
    tmpStyle = tr("EN/")+ QLocale::system().name().mid(0, 2).toUpper();
    if (this->isEmbeddedKeyboard() == true)
        tmpStyle = tmpStyle.toLower();
    tmp = createNewKey(tmpStyle);
    tmp->setMaximumWidth(tmp->maximumWidth());
    tmp->setMinimumWidth(tmp->minimumWidth());
    tmpLayout->addWidget(tmp);
#if !defined(QT_ARCH_MACOSX)
    //tmpLayout->insertStretch(13, 1);
#endif
	tmpVLayout->insertLayout(0, tmpLayout);
	//
	// Stampa linea della "Q":
	tmpLayout = new QHBoxLayout;	
    tmpLayout->setSpacing(spacing);
    QVBoxLayout *layoutReturn = new QVBoxLayout;
    layoutReturn->setSpacing(spacing);
    tmp = createNewKey(KEY_TAB);
    tmp->setMaximumWidth(tmp->maximumWidth() * 2 - 10);
    tmp->setMinimumWidth(tmp->minimumWidth() * 2 - 10);
    tmpLayout->addWidget(tmp);
    tmpLayout->addWidget(createNewKey(tr("Q")));
    tmpLayout->addWidget(createNewKey(tr("W")));
    tmpLayout->addWidget(createNewKey(tr("E")));
    tmpLayout->addWidget(createNewKey(tr("R")));
    tmpLayout->addWidget(createNewKey(tr("T")));
    tmpLayout->addWidget(createNewKey(tr("Y")));
    tmpLayout->addWidget(createNewKey(tr("U")));
    tmpLayout->addWidget(createNewKey(tr("I")));
    tmpLayout->addWidget(createNewKey(tr("O")));
    tmpLayout->addWidget(createNewKey(tr("P")));
    tmpLayout->addWidget(createNewKey(tr("+")));
    tmpLayout->addWidget(createNewKey(tr("*")));
    tmpLayout->addWidget(createNewKey(tr("-")));
#if !defined(QT_ARCH_MACOSX)
    //tmpLayout->insertStretch(-1, 1);
#endif
    layoutReturn->insertLayout(0, tmpLayout, 1); // inserisce la riga della "Q"
	//
	// Stampa linea della "A":
    tmpLayout = new QHBoxLayout;
    tmpLayout->setSpacing(spacing);
	tmp = createNewKey(KEY_CAPS);
    tmp->setMaximumWidth(tmp->maximumWidth() * 2 + 10);
    tmp->setMinimumWidth(tmp->minimumWidth() * 2 + 10);
    if (this->isEmbeddedKeyboard() == true)
        tmpStyle = QString(EMBEDDED_KEYBOARD);
    tmp->setStyleSheet(QString(DEFAULT_STYLE_BUTTON) + QString(CHANGED_BACKGROUND_BUTTON) + tmpStyle); // segnalato come attivo
	tmpLayout->addWidget(tmp);
    tmpLayout->addWidget(createNewKey(tr("A")));
    tmpLayout->addWidget(createNewKey(tr("S")));
    tmpLayout->addWidget(createNewKey(tr("D")));
    tmpLayout->addWidget(createNewKey(tr("F")));
    tmpLayout->addWidget(createNewKey(tr("G")));
    tmpLayout->addWidget(createNewKey(tr("H")));
    tmpLayout->addWidget(createNewKey(tr("J")));
    tmpLayout->addWidget(createNewKey(tr("K")));
    tmpLayout->addWidget(createNewKey(tr("L")));
    tmpLayout->addWidget(createNewKey(tr(";")));
    tmpLayout->addWidget(createNewKey(tr(":")));
    tmpLayout->addWidget(createNewKey(tr("Ã¹")));
    tmpLayout->addWidget(createNewKey(tr("@")));
    //tmpLayout->insertStretch(-1, 1);
#if !defined(QT_ARCH_MACOSX)
    layoutReturn->setSpacing(spacing);
#endif
    layoutReturn->setSpacing(spacing);
    layoutReturn->insertLayout(1, tmpLayout, 1); // inserisce la riga della "A"
	//
	// inserisce il vertical layout all'interno di un horizontal:	
	tmpLayout = new QHBoxLayout;
    tmpLayout->setSpacing(spacing);
    tmpLayout->insertLayout(0, layoutReturn, 1);
	//
	// inserisce anche il tasto invio:
    //tmp = createNewKey(KEY_RETURN);
    tmp = createNewKey(tr("RETURN"));
    tmp->setMaximumWidth(tmp->maximumWidth() * 2);
	tmp->setMinimumWidth(tmp->minimumWidth() * 2);
	tmp->setMinimumHeight(tmp->minimumHeight() * 2);
	tmp->setMaximumHeight(tmp->maximumHeight() * 2);
	tmpLayout->addWidget(tmp);
	//
    // inserisce l'horizontal all'interno del layout verticale principale:
    tmpVLayout->insertLayout(1, tmpLayout);
	//	
	// Stampa linea della "Z":
	tmpLayout = new QHBoxLayout;
    tmpLayout->setSpacing(spacing);
    tmp = createNewKey(KEY_CUT_LEFT);
	tmp->setMaximumWidth(tmp->maximumWidth() * 2);
	tmp->setMinimumWidth(tmp->minimumWidth() * 2);
	tmpLayout->addWidget(tmp);
    tmpLayout->addWidget(createNewKey(tr("_")));
    tmpLayout->addWidget(createNewKey(tr("Z")));
    tmpLayout->addWidget(createNewKey(tr("X")));
    tmpLayout->addWidget(createNewKey(tr("C")));
    tmpLayout->addWidget(createNewKey(tr("V")));
    tmpLayout->addWidget(createNewKey(tr("B")));
    tmpLayout->addWidget(createNewKey(tr("N")));
    tmpLayout->addWidget(createNewKey(tr("M")));
    tmpLayout->addWidget(createNewKey(tr("(")));
    tmpLayout->addWidget(createNewKey(tr(")")));
    tmpLayout->addWidget(createNewKey(tr(",")));
    tmpLayout->addWidget(createNewKey(tr(".")));
    tmp = createNewKey(KEY_CTRL_LEFT);
    tmp->setMaximumWidth(tmp->maximumWidth() * 3 + 15);
    tmp->setMinimumWidth(tmp->minimumWidth() * 3 + 15);
	tmpLayout->addWidget(tmp);
	tmpVLayout->insertLayout(2, tmpLayout);
	//	
	// Stampa linea dello SPACE:
	tmpLayout = new QHBoxLayout;
    tmpLayout->setSpacing(spacing);
    tmp = createNewKey(KEY_COPY);
	tmp->setMaximumWidth(tmp->maximumWidth() * 2);
	tmp->setMinimumWidth(tmp->minimumWidth() * 2);
	tmpLayout->addWidget(tmp);
	tmp = createNewKey(KEY_ALT);
	tmp->setMaximumWidth(tmp->maximumWidth() * 2);
	tmp->setMinimumWidth(tmp->minimumWidth() * 2);
	tmpLayout->addWidget(tmp);
    tmp = createNewKey(KEY_SPACE);
	tmp->setMaximumWidth(tmp->maximumWidth() * 10);
	tmp->setMinimumWidth(tmp->minimumWidth() * 10);
	tmpLayout->addWidget(tmp);
    // password echo button:
    if (this->isEmbeddedKeyboard() == true)
        tmpStyle = QString(KEY_HIDECHAR_EMBEDDED);
    else
        tmpStyle = QString(KEY_HIDECHAR);
    tmp = createNewKey(tmpStyle);
	tmp->setMaximumWidth(tmp->maximumWidth() * 2);
	tmp->setMinimumWidth(tmp->minimumWidth() * 2);
	tmpLayout->addWidget(tmp);
    //
    tmp = createNewKey(KEY_PASTE);
	tmp->setMaximumWidth(tmp->maximumWidth() * 2);
	tmp->setMinimumWidth(tmp->minimumWidth() * 2);
	tmpLayout->addWidget(tmp);
	tmpVLayout->insertLayout(3, tmpLayout);
#if defined (QT_ARCH_MACOSX)
    tmpVLayout->setMargin(5);
#endif
    // aggancia il layout a tutto il form:
    this->setLayout(tmpVLayout);
    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void widgetKeyBoard::recreateKeyboard()
{
    if (widgetKeyBoard::m_created == false) // tastiera già  creata: esce
        return;
    widgetKeyBoard::m_created = false; // segnala che è stata creata la tastiera
    delete this->layout();
    createKeyboard();

}
