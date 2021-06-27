




























#include "gui/config_dialog/simulkana_table_editor.h"

#include <QtGui/QtGui>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMessageBox>

#include <cctype>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "base/config_file_stream.h"
#include "base/logging.h"
#include "base/util.h"
#include "gui/base/table_util.h"
#include "gui/base/util.h"
#include "protocol/commands.pb.h"

namespace mozc {
namespace gui {
namespace {
enum {
  NEW_INDEX = 0,
  REMOVE_INDEX = 1,
  IMPORT_FROM_FILE_INDEX = 2,
  EXPORT_TO_FILE_INDEX = 3,
  RESET_INDEX = 4,
  MENU_SIZE = 5
};
const char kSimulKanaTableFile[] = "system://shingeta.tsv";
}  // namespace

SimulKanaTableEditorDialog::SimulKanaTableEditorDialog(QWidget *parent)
    : GenericTableEditorDialog(parent, 4) {
  actions_.reset(new QAction *[MENU_SIZE]);
  actions_[NEW_INDEX] = mutable_edit_menu()->addAction(tr("New entry"));
  actions_[REMOVE_INDEX] =
      mutable_edit_menu()->addAction(tr("Remove selected entries"));
  mutable_edit_menu()->addSeparator();
  actions_[IMPORT_FROM_FILE_INDEX] =
      mutable_edit_menu()->addAction(tr("Import from file..."));
  actions_[EXPORT_TO_FILE_INDEX] =
      mutable_edit_menu()->addAction(tr("Export to file..."));
  mutable_edit_menu()->addSeparator();
  actions_[RESET_INDEX] =
      mutable_edit_menu()->addAction(tr("Reset to defaults"));

  setWindowTitle(tr("[ProductName] SimulKana table editor"));
  GuiUtil::ReplaceWidgetLabels(this);
  dialog_title_ = GuiUtil::ReplaceString(tr("[ProductName] settings"));
  CHECK(mutable_table_widget());
  CHECK_EQ(mutable_table_widget()->columnCount(), 4);
  QStringList headers;
  headers << tr("Key1") << tr("Key2") << tr("Output") << tr("Limit (ms)");
  mutable_table_widget()->setHorizontalHeaderLabels(headers);

  resize(440, 450);

  UpdateMenuStatus();
}

SimulKanaTableEditorDialog::~SimulKanaTableEditorDialog() {}

std::string SimulKanaTableEditorDialog::GetDefaultSimulKanaTable() {
  std::unique_ptr<std::istream> ifs(
      ConfigFileStream::LegacyOpen(kSimulKanaTableFile));
  CHECK(ifs.get() != nullptr);  // should never happen
  std::string line, result;
  std::vector<std::string> fields;
  while (std::getline(*ifs, line)) {
    if (line.empty()) {
      continue;
    }
    Util::ChopReturns(&line);
    fields.clear();
    Util::SplitStringAllowEmpty(line, "\t", &fields);
    if (fields.size() < 3) {
      VLOG(3) << "field size < 3";
      continue;
    }
    result += fields[0];
    result += '\t';
    result += fields[1];
    result += '\t';
    result += fields[2];
    if (fields.size() >= 4) {
      result += '\t';
      result += fields[3];
    }
    result += '\n';
  }
  return result;
}

bool SimulKanaTableEditorDialog::LoadFromStream(std::istream *is) {
  CHECK(is);
  std::string line;
  std::vector<std::string> fields;
  mutable_table_widget()->setRowCount(0);
  mutable_table_widget()->verticalHeader()->hide();

  int row = 0;
  while (std::getline(*is, line)) {
    if (line.empty()) {
      continue;
    }
    Util::ChopReturns(&line);

    fields.clear();
    Util::SplitStringAllowEmpty(line, "\t", &fields);
    if (fields.size() < 3) {
      VLOG(3) << "field size < 3";
      continue;
    }

    if (fields.size() == 3) {
      fields.push_back("");
    }

    QTableWidgetItem *key1 =
        new QTableWidgetItem(QString::fromUtf8(fields[0].c_str()));
    QTableWidgetItem *key2 =
        new QTableWidgetItem(QString::fromUtf8(fields[1].c_str()));
    QTableWidgetItem *output =
        new QTableWidgetItem(QString::fromUtf8(fields[2].c_str()));
    QTableWidgetItem *simullimit =
        new QTableWidgetItem(QString::fromUtf8(fields[3].c_str()));

    mutable_table_widget()->insertRow(row);
    mutable_table_widget()->setItem(row, 0, key1);
    mutable_table_widget()->setItem(row, 1, key2);
    mutable_table_widget()->setItem(row, 2, output);
    mutable_table_widget()->setItem(row, 3, simullimit);
    ++row;

    if (row >= max_entry_size()) {
      QMessageBox::warning(
          this, dialog_title_,
          tr("You can't have more than %1 entries").arg(max_entry_size()));
      break;
    }
  }

  UpdateMenuStatus();

  return true;
}

bool SimulKanaTableEditorDialog::LoadDefaultSimulKanaTable() {
  std::unique_ptr<std::istream> ifs(
      ConfigFileStream::LegacyOpen(kSimulKanaTableFile));
  CHECK(ifs.get() != nullptr);  // should never happen
  CHECK(LoadFromStream(ifs.get()));
  return true;
}

bool SimulKanaTableEditorDialog::Update() {
  if (mutable_table_widget()->rowCount() == 0) {
    QMessageBox::warning(this, dialog_title_,
                         tr("SimulKana table is empty."));
    return false;
  }

  std::unordered_map<std::string, std::string> intermediate_comb_table;
  bool contains_capital = false;
  std::string *table = mutable_table();
  table->clear();
  for (int i = 0; i < mutable_table_widget()->rowCount(); ++i) {
    const std::string &key1 =
        TableUtil::SafeGetItemText(mutable_table_widget(), i, 0).toStdString();
    const std::string &key2 =
        TableUtil::SafeGetItemText(mutable_table_widget(), i, 1).toStdString();
    const std::string &output =
        TableUtil::SafeGetItemText(mutable_table_widget(), i, 2).toStdString();
    const std::string &simullimit =
        TableUtil::SafeGetItemText(mutable_table_widget(), i, 3).toStdString();
    if (key1.empty() || output.empty()) {
      continue;
    }
    if (key2.empty()) {
      *table += key1;
      *table += '\t';
      *table += '\t';
      *table += output;
      if (!simullimit.empty()) {
        *table += '\t';
        *table += simullimit;
      }
      *table += '\n';
    } else { // if key2 is present, permutation needs to happen
      if (key1.compare(key2)==0) { // if key1==key2, ignore this entry
        continue;
      }
      // please note that whatever comes at first will be taken
      if (intermediate_comb_table[key1 + '\t' + key2].empty()) {
        intermediate_comb_table[key1 + '\t' + key2] = output + '\t' + simullimit;
        *table += key1;
        *table += '\t';
        *table += key2;
        *table += '\t';
        *table += output;
        if (!simullimit.empty()) {
          *table += '\t';
          *table += simullimit;
        }
        *table += '\n';
      }
      // start of consistency check
      if (intermediate_comb_table[key2 + '\t' + key1].empty()) { // reversed combination does not exist yet..
        *table += key2;
        *table += '\t';
        *table += key1;
        *table += '\t';
        *table += output;
        if (!simullimit.empty()) {
          *table += '\t';
          *table += simullimit;
        }
        *table += '\n';
        intermediate_comb_table[key2 + '\t' + key1] = output + '\t' + simullimit;
      }
    }

    if (!contains_capital) {
      std::string lower1 = key1;
      Util::LowerString(&lower1);
      std::string lower2 = key2;
      Util::LowerString(&lower2);
      contains_capital = ((lower1 != key1) || (lower2 != key2));
    }
  }

  if (contains_capital) {
    // TODO(taku):
    // Want to see the current setting and suppress this
    // dialog if the shift-mode-switch is already off.
    QMessageBox::information(this, dialog_title_,
                             tr("Input fields contain capital characters. "
                                "\"Shift-mode-switch\" function is disabled "
                                "with this new mapping."));
  }

  return true;
}

void SimulKanaTableEditorDialog::UpdateMenuStatus() {
  const bool status = (mutable_table_widget()->rowCount() > 0);
  actions_[RESET_INDEX]->setEnabled(status);
  actions_[REMOVE_INDEX]->setEnabled(status);
  UpdateOKButton(status);
}

void SimulKanaTableEditorDialog::OnEditMenuAction(QAction *action) {
  if (action == actions_[NEW_INDEX]) {
    AddNewItem();
  } else if (action == actions_[REMOVE_INDEX]) {
    DeleteSelectedItems();
  } else if (action == actions_[IMPORT_FROM_FILE_INDEX] ||
             action == actions_[RESET_INDEX]) {  // import or reset
    if (mutable_table_widget()->rowCount() > 0 &&
        QMessageBox::Ok !=
            QMessageBox::question(
                this, dialog_title_,
                tr("Do you want to overwrite the current roman table?"),
                QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel)) {
      return;
    }

    if (action == actions_[IMPORT_FROM_FILE_INDEX]) {
      Import();
    } else if (action == actions_[RESET_INDEX]) {
      LoadDefaultSimulKanaTable();
    }
  } else if (action == actions_[EXPORT_TO_FILE_INDEX]) {
    Export();
  }
}

// static
bool SimulKanaTableEditorDialog::Show(QWidget *parent,
                                  const std::string &current_roman_table,
                                  std::string *new_roman_table) {
  SimulKanaTableEditorDialog window(parent);

  if (current_roman_table.empty()) {
    window.LoadDefaultSimulKanaTable();
  } else {
    window.LoadFromString(current_roman_table);
  }

  // open modal mode
  const bool result = (QDialog::Accepted == window.exec());
  new_roman_table->clear();

  if (result && window.table() != window.GetDefaultSimulKanaTable()) {
    *new_roman_table = window.table();
  }

  return result;
}
}  // namespace gui
}  // namespace mozc
