#include "dictionary.hpp"

const Trie& Dictionary::get_dictionary() const { return m_trie; }

std::vector<std::string> Dictionary::get_suggestions(
    const std::string& word) const {
  std::set<std::string> suggestions;

  for (int i = 0; i < word.length(); ++i) {
    for (char ch = 'a'; ch <= 'z'; ++ch) {
      std::string modified_word = word;
      modified_word[i] = ch;
      if (!get_dictionary().search(modified_word)) {
        continue;
      }

      if (levenshtein::calculate_distance(modified_word, word) >
          LEVENSHTEIN_MAX_DISTANCE) {
        continue;
      }

      suggestions.insert(modified_word);
    }
  }

  for (int i = 0; i < word.length(); ++i) {
    for (char ch = 'a'; ch <= 'z'; ++ch) {
      std::string modified_word = word;
      modified_word.insert(i, 1, ch);
      if (!get_dictionary().search(modified_word)) {
        continue;
      }

      if (levenshtein::calculate_distance(modified_word, word) >
          LEVENSHTEIN_MAX_DISTANCE) {
        continue;
      }

      suggestions.insert(modified_word);
    }
  }

  for (int i = 0; i < word.length(); ++i) {
    std::string modified_word = word;
    modified_word.erase(i, 1);
    if (!get_dictionary().search(modified_word)) {
      continue;
    }

    if (levenshtein::calculate_distance(modified_word, word) >
        LEVENSHTEIN_MAX_DISTANCE) {
      continue;
    }

    suggestions.insert(modified_word);
  }

  for (int i = 0; i < word.length() - 1; ++i) {
    std::string modified_word = word;
    std::swap(modified_word[i], modified_word[i + 1]);
    if (!get_dictionary().search(modified_word)) {
      continue;
    }

    if (levenshtein::calculate_distance(modified_word, word) >
        LEVENSHTEIN_MAX_DISTANCE) {
      continue;
    }

    suggestions.insert(modified_word);
  }

  for (int i = 1; i < word.length() + 1; ++i) {
    std::string modified_word = word.substr(0, i);
    if (!get_dictionary().search(modified_word)) {
      continue;
    }

    if (levenshtein::calculate_distance(modified_word, word) >
        LEVENSHTEIN_MAX_DISTANCE) {
      continue;
    }

    if (modified_word.length() < MIN_LENGTH_OF_WORD) {
      continue;
    }

    suggestions.insert(modified_word);
  }

  std::vector<std::string> sorted_suggestions{
      suggestions.begin(), std::next(suggestions.begin(), 5)};

  levenshtein::Comparator comparator;
  comparator.set_target(word);

  std::sort(sorted_suggestions.begin(), sorted_suggestions.end(), comparator);

  return sorted_suggestions;
}

void Dictionary::load_from_file(std::string_view filename) {
  std::ifstream input_file(filename.data(), std::ios::binary);

  if (!input_file.is_open()) {
    std::cerr << "Could not open dictionary file: " << filename << std::endl;
    return;
  }

  std::cout << "Reading dictionary file..." << std::endl;

  std::string word;
  while (std::getline(input_file, word)) {
    m_trie.insert(word);
  }

  if (!input_file.eof() && input_file.fail()) {
    std::cerr << "Error while reading dictionary file: " << filename
              << std::endl;
    return;
  }

  std::cout << "Completed." << std::endl;

  input_file.close();
}
