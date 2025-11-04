#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_WORD_COUNT 15'000
#define MAX_SUCCESSOR_COUNT MAX_WORD_COUNT / 2

char book[] = {
#embed "frankenstain.txt"
    , '\0'};

/// Array of tokens registered so far
/// No duplicates are allowed.
char *tokens[MAX_WORD_COUNT];
/// `tokens`'s current size
size_t tokens_size = 0;

/// Array of successor tokens
/// One token can have many successor tokens. `succs[x]` corresponds to
/// `token[x]`'s successors.
/// We store directly tokens instead of token_ids, because we will directly
/// print them. If we wanted to delete the book, then it would make more sense
/// to store `token_id`s
char *succs[MAX_WORD_COUNT][MAX_SUCCESSOR_COUNT];
/// `succs`'s size
size_t succs_sizes[MAX_WORD_COUNT];

/// Overwrites non-printable characters in `book` with a space.
/// Non-printable characters may lead to duplicates like
/// `"\xefthe" != "the"` even both print `the`.
void replace_non_printable_chars_with_space() {
  // my code
  for (size_t i = 0; i < strlen(book); i++) { // går i gennem alle karektere
    if (!isprint(book[i])) {                  // ser om karektere er printebel
      book[i] = ' '; // hvis karektere ikke er printebel ersates den med " "
    }
  }

  // my code end
}

/// Returns token id if token is known, otherwise creates a new entry and
/// returns the new token id
size_t token_id(char *token) {
  size_t id;
  for (id = 0; id < tokens_size; ++id) {
    if (strcmp(tokens[id], token) == 0) {
      return id;
    }
  }
  tokens[id] = token;
  ++tokens_size;
  return id;
}

/// Appends a token to the successors list of a `token`
void append_to_succs(char *token, char *succ) {
  auto next_empty_index_ptr = &succs_sizes[token_id(token)];

  if (*next_empty_index_ptr >= MAX_SUCCESSOR_COUNT) {
    printf("Successor array full.");
    exit(EXIT_FAILURE);
  }

  succs[token_id(token)][(*next_empty_index_ptr)++] = succ;
}

/// Creates tokens from the `book` and fills `tokens` and `succs` arrays using
/// the `token`s.
void tokenize_and_fill_succs(char *delimiters, char *str) {
  // my code
  size_t ord[MAX_WORD_COUNT] =
      {}; // et array som har alle ordne i tektesn ersattet med deres token id
  // token array generation
  char *token; // satter variablen *token* som er bruges til at holde et enkelt
               // token

  // stater en varibe *ord_len* somer den akruelle længden af  *ord*
  int ord_len = 0;

  // laver den første token
  // her fortæller vi *strtok* hvilken string den skal arbejde med, den laver så
  // en string af alle karekternde inden den møder em "delimiter" som er sætnigs
  // afslutere
  token = strtok(str, delimiters);

  // et loop der søgre for at vi laver token af hele bogen
  while (token) { // hvis *strtok* sætter *token* til NULL stopper den. dette
                  // sker når *strtok* ikke har meer at arbejde med

    // tejker om *ord_len* er noget sin maximum størelse
    if (ord_len >= MAX_WORD_COUNT) {
      break; // af bryder while loopet
    }

    token_id(token); // kalder *token_id* med *token* som input
    ord[ord_len] =
        token_id(token); // sætter palds *ord_len* i *ord* til *token*'s id. der
                         // med skaber vi bogen skrevet i token ID
    ord_len++;           // incremitere *ord_len*
    // sætter token til den næste string i bogen
    token = strtok(NULL, delimiters); // null her beyder at den skal fortsæte
                                      // med at abejde med den tidligre string
  };

  //  her skaber vi sucsser arrayet
  // et loop der tjeker hvilke ord der kommer efter et givet token
  for (size_t i = 1; i < ord_len;
       i++) { // løber alle tokens igenem undtagen token 0 da vi satter i 1
    size_t forrige_ord =
        ord[i - 1]; // ser hvilke tokens der er der kommer før *token[i]* i ord
    size_t successor_ord = ord[i]; // sætter en variabel til *ord[i]*

    // hvis det maximale antal af successor til *ord[i-1]* er nåt skiper vi
    // ord[i]
    if (succs_sizes[forrige_ord] >= MAX_SUCCESSOR_COUNT) {
      continue; // siger at vi skal skippe resten men fortsæte i for loppet
    }
    // kalder funktioen der sætter *successor_ord* som *forrige_ord*
    append_to_succs(tokens[forrige_ord], tokens[successor_ord]);
  }
  // my code end
}

/// Returns last character of a string
char last_char(char *str) {
  // my code
  return (str[strlen(str) - 1]);
  // my code end
}

/// Returns whether the token ends with `!`, `?` or `.`.
bool token_ends_a_sentence(char *token) {
  // my code
  // hvis den sidste krarekter er entel !, ? eller . retundere den falsk eller
  // retunere den falsk
  if (last_char(token) == '!' || last_char(token) == '?' ||
      last_char(token) == '.') {
    return true;
  }
  return false;
  // my code end
}

/// Returns a random `token_id` that corresponds to a `token` that starts with a
/// capital letter.
/// Uses `tokens`, `tokens_size.
size_t random_token_id_that_starts_a_sentence() {
  // my code
  size_t token_id;
  bool found = false;
  size_t start_token;

  while (!found) {

    token_id = rand() % tokens_size;

    if (isupper(tokens[token_id][0])) {
      start_token = token_id;
      found = true;
    }
  }

  return start_token;
}

/// Generates a random sentence using `tokens`, `succs`, and `succs_sizes`.
/// The sentence array will be filled up to `sentence_size-1` characters using
/// random tokens until:
/// - a token is found where `token_ends_a_sentence()`
/// - or more tokens cannot be concatenated to the `sentence` anymore.
///  Returns the filled sentence array.
char *generate_sentence(char *sentence, size_t sentence_size) {

  size_t current_token_id = random_token_id_that_starts_a_sentence();
  auto token = tokens[current_token_id];

  sentence[0] = '\0';
  strcat(sentence, token);
  if (token_ends_a_sentence(token))
    return sentence;

  // Calculated sentence length for the next iteration.
  // Used to stop the loop if the length exceeds sentence size
  size_t sentence_len_next;
  // Concatenates random successors to the sentence as long as
  // `sentence` can hold them.
  do {
    // my code

    char *efter_token; // satrter en variabel der skal lægre havd der kommer
                       // efter *token*

    // hvis *token* ikke har nogen succesesors bruder vi ud af while loopet
    if (succs_sizes[token_id(token)] == 0) {
      break;
    }

    // generare en til fældigt succesesors ved at genere et til fældigt tal og
    // moduluse det med antalet af succesesors for *token*
    size_t random_succs_id = rand() % succs_sizes[token_id(token)];

    // sæter *efter_token* til den generarede succesesors
    efter_token = succs[token_id(token)][random_succs_id];

    // tjeker om vi kommer til at over skirde *sentence_size* ved at ligge
    // *efter_token* og 2 (for mellemrum og NULL terminater) til den nu værende
    // sentence
    if (strlen(sentence) + strlen(efter_token) + 2 == sentence_size) {
      break; // bryder ud af while loopet *sentence_size* er blevet over skedet
    } else {
      strcat(sentence, " "); // sætter mellemrum mellem alle ord

      // sætter *efter_token* bagers i den nuværnde sætning
      strcat(sentence, efter_token);
    }

    // tejker om *efter_token* har en sætnings aflsuter i sig
    if (token_ends_a_sentence(efter_token)) {
      break; // hvis den har det bryder vi ud af while loopet
    }
    // sætter *sentence_len_next* til længden af *sentence*
    sentence_len_next = strlen(sentence);
    token = efter_token; // sæter *token* til *efter_token* så procesen er kalr
                         // til at køre igen
    // my code end
  } while (sentence_len_next < sentence_size - 1);
  return sentence;
}

int main() {
  replace_non_printable_chars_with_space();

  char *delimiters = " \n\r";
  tokenize_and_fill_succs(delimiters, book);

  char sentence[1000];
  srand(time(NULL));

  // Initialize `sentence` and then find a question sentence.
  do {
    generate_sentence(sentence, sizeof sentence);
  } while (last_char(sentence) != '?');

  puts(sentence);
  puts("");

  // Initialize `sentence` and then find a sentence ending with exclamation
  // mark.
  do {
    generate_sentence(sentence, sizeof sentence);
  } while (last_char(sentence) != '!');
  puts(sentence);
}