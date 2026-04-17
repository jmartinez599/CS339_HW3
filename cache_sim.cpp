#include<iostream>
#include<fstream>
#include<cstdlib>
#include<cmath>
#include<string>

using namespace std;

class Entry {
public:
  Entry();
  ~Entry();
  void display(ofstream& outfile);

  void set_tag(int _tag) { tag = _tag; }
  int get_tag() { return tag; }

  void set_valid(bool _valid) { valid = _valid; }
  bool get_valid() { return valid; }

  void set_ref(int _ref) { ref = _ref; }
  int get_ref() { return ref; }

private:  
  bool valid;
  unsigned tag;
  int ref;
};

Entry::Entry() : valid(false), tag(0), ref(0) {}
Entry::~Entry() {}

void Entry::display(ofstream& outfile) {
    outfile << "valid=" << valid << " tag=" << tag << " ref=" << ref;
}

class Cache {
public:
  Cache(int num_entries, int associativity);
  ~Cache();

  void display(ofstream& outfile);


  int get_index(unsigned long addr);
  int get_tag(unsigned long addr);

  unsigned long retrieve_addr(int way, int index);
  
  bool hit(ofstream& outfile, unsigned long addr);

  void update(ofstream& outfile, unsigned long addt);

  
private:
  int assoc;
  unsigned num_entries;
  int num_sets;
  int index_bits;
  int global_ref;
  Entry **entries;
};

Cache::Cache(int num_entries_, int associativity) 
    : assoc(associativity), 
      num_entries(num_entries_),
      global_ref(0)
      {
        num_sets = num_entries / assoc;
        index_bits = (num_sets > 1) ? (int)log2(num_sets) : 0;

        entries = new Entry*[assoc];
        for (int w = 0; w < assoc; w++)
            entries[w] = new Entry[num_sets];
      }

Cache::~Cache() {
  for (int w = 0; w < assoc; w++)
    delete[] entries[w];
  delete[] entries;
}

void Cache::display(ofstream& outfile) {
  for (int s = 0; s < num_sets; s++) {
    outfile << "Set " << s << ": ";
    for (int w = 0; w < assoc; w++) {
      outfile << "[way" << w << " ";
      entries[w][s].display(outfile);
      outfile << "] ";
    }
    outfile << endl;
  }
}

int Cache::get_index(unsigned long addr) {
    if(num_sets==1) return 0;
    return(int)(addr & (unsigned long)(num_sets - 1));
}

int Cache::get_tag(unsigned long addr){
    return(int)(addr >> index_bits);
}

unsigned long Cache::retrieve_addr(int way, int index){
    unsigned long t = (unsigned long)entries[way][index].get_tag();
    return(t << index_bits) | (unsigned long)index;
}

bool Cache::hit(ofstream& outfile, unsigned long addr) {
  int idx = get_index(addr);
  int tag = get_tag(addr);

  for (int w = 0; w < assoc; w++) {
    if (entries[w][idx].get_valid() && entries[w][idx].get_tag() == tag) {
      outfile << addr << " : HIT" << endl;
      entries[w][idx].set_ref(global_ref++);  // refresh LRU timestamp
      return true;
    }
  }
  outfile << addr << " : MISS" << endl;
  return false;
}

void Cache::update(ofstream& outfile, unsigned long addr) {
  int idx = get_index(addr);
  int tag = get_tag(addr);

  // First: look for an invalid (empty) slot
  for (int w = 0; w < assoc; w++) {
    if (!entries[w][idx].get_valid()) {
      entries[w][idx].set_tag(tag);
      entries[w][idx].set_valid(true);
      entries[w][idx].set_ref(global_ref++);
      return;
    }
  }

  int lru_way = 0;
  for (int w = 1; w < assoc; w++) {
    if (entries[w][idx].get_ref() < entries[lru_way][idx].get_ref())
      lru_way = w;
  }
  entries[lru_way][idx].set_tag(tag);
  entries[lru_way][idx].set_ref(global_ref++);
  // valid stays true — we're replacing, not invalidating
}



int main(int argc, char* argv[]) {
  if (argc < 4) {
    cout << "Usage:" << endl;
    cout << "   ./cache_sim num_entries associativity filename" << endl;
    return 0;
  }

  unsigned entries = atoi(argv[1]);
  unsigned assoc   = atoi(argv[2]);
  string input_filename  = argv[3];
  string output_filename = "cache_sim_output";

  ifstream input;
  ofstream output;

  input.open(input_filename);
  if (!input.is_open()) {
    cerr << "Could not open input file " << input_filename << ". Exiting ..." << endl;
    exit(0);
  }
  output.open(output_filename);

  Cache cache((int)entries, (int)assoc);

  unsigned long addr;
  while (input >> addr) {
    if (!cache.hit(output, addr))
      cache.update(output, addr);
  }

  input.close();
  output.close();
  return 0;
}
