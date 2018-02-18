#include "Article.h"

Article::Article(int table_size, int h1_param, int h2_param)
    : table_size(table_size), h1_param(h1_param), h2_param(h2_param) {
  n = 0;
  table = new std::pair<std::string, int>[table_size];
  for (int i = 0; i < table_size; i++) {
    table[i].first = EMPTY_KEY;
    table[i].second = EMPTY_INDEX;
  }
}

Article::~Article() {
  n = 0;
  table_size = 0;
  h1_param = 0;
  h2_param = 0;
  delete[] table;
  table = NULL;
}

int Article::get(std::string key, int nth, std::vector<int> &path) const {
  int index = 0;
  for (int i = 0; i < table_size and 0 < nth; i++) {
    index = hash_function(key, i);
    if (i != 0)
      path.push_back(index);
    if (table[index].first == EMPTY_KEY && table[index].second == EMPTY_INDEX)
      break;
    else if (table[index].first == key)
      nth--;
  }
  if (nth == 0 && table[index].first == key)
    return table[index].second;
  else
    return -1;
}

int Article::insert(std::string key, int original_index) {
  int index = 0;
  int probing_count = 0;
  bool dont_count = false;
  if (getLoadFactor() > MAX_LOAD_FACTOR)
    expand_table();
  for (int i = 0; i < table_size; i++) {
    index = hash_function(key, i);
    if (i != 0 && !dont_count)
      probing_count++;
    if (table[index].first == EMPTY_KEY) {
      table[index].first = key;
      table[index].second = original_index;
      n++;
      break;
    } else if (table[index].first == key && table[index].second > original_index) {
      int temp = table[index].second;
      table[index].second = original_index;
      original_index = temp;
      dont_count = true;
    }
  }
  return probing_count;
}

int Article::remove(std::string key, int nth) {
  int index = 0;
  int probing_count = 0;
  for (int i = 0; i < table_size && 0 < nth; i++) {
    index = hash_function(key, i);
    if (i != 0)
      probing_count++;
    if (table[index].first == EMPTY_KEY && table[index].second == EMPTY_INDEX)
      break;
    else if (table[index].first == key)
      nth--;
  }
  if (nth == 0 && table[index].first == key) {
    n--;
    table[index].first = EMPTY_KEY;
    table[index].second = MARKED_INDEX;
    return probing_count;
  }
  return -1;
}

double Article::getLoadFactor() const {
  return ((double) n) / ((double) table_size);
}

void Article::getAllWordsFromFile(std::string filepath) {
  std::ifstream input_file;
  std::string word;
  int index = 1;
  input_file.open(filepath.c_str());
  while (input_file >> word) {
    insert(word, index);
    index++;
  }
  input_file.close();
}

void Article::expand_table() {
  std::pair<std::string, int> *old_table = table;
  int old_table_size = table_size;
  n = 0;
  table_size = nextPrimeAfter(2 * old_table_size);
  h2_param = firstPrimeBefore(table_size);
  table = new std::pair<std::string, int>[table_size];
  for (int i = 0; i < table_size; i++) {
    table[i].first = EMPTY_KEY;
    table[i].second = EMPTY_INDEX;
  }
  for (int i = 0; i < old_table_size; i++) {
    if (old_table[i].first != EMPTY_KEY) {
      insert(old_table[i].first, old_table[i].second);
    }
  }
  delete[] old_table;
  return;
}

int Article::hash_function(std::string &key, int i) const {
  int int_key = convertStrToInt(key);
  int hash_val = (h1(int_key) + i * h2(int_key)) % table_size;
  if (hash_val < 0)
    hash_val += table_size;
  return hash_val;
}

int Article::h1(int key) const {
  int pop_count = 0;
  while (key) {
    pop_count += key % 2;
    key /= 2;
  }
  return pop_count * h1_param;
}

int Article::h2(int key) const {
  return h2_param - (key % h2_param);
}