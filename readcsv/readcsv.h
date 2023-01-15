
#ifndef READCSV_H
#define READCSV_H


namespace csv {
  class cell {
    public:
    std::uintmax_t start, end;
    void reset () { start=end=0; }
    cell () { start=end=0; }
  };

  class row {
    public:
    std::vector<cell> cells;
    std::uintmax_t start, end;
    void reset () { cells.clear(); start=end=0; }
    row () { start=end=0; }
  };

  class file {
    private:
    std::vector<row> rows;

    char c;
    std::uintmax_t curr_pos=0;
    row curr_row;
    cell curr_cell;
    bool currently_escaped=false, currently_delimited=false, loaded_in_mem;
    std::string filename;
    std::string in_mem="";

    void parse_file(char c);
    void end_parse_file();
    
    public:
    file (std::string _filename="")  { filename=_filename; };
    ~file ()  { };
    std::string get_filename() { return filename; };

    bool is_csv=true;
    char cell_separator=';', string_delimiter='\0', end_of_line='\n', escape='\\';
    size_t max_line_count=256, min_cell_size=8, max_cell_size=256;

    void reset();
    bool read_from_file();
    bool read_in_memory();
    bool load(std::string _filename, bool in_memory=true);

    void stat(bool metadata_lines=true);

    std::string read_substring_from_file(std::uintmax_t start_read=0, std::uintmax_t length_read=0);

    bool parse_range(std::string, std::vector<std::uintmax_t>&);
    bool parse_list(std::string, std::vector<std::uintmax_t>&);

    void list_row(std::uintmax_t r1, std::uintmax_t r2);
    void list_row(std::uintmax_t r);
    void list_row();

    std::uintmax_t cell_count();
    bool get_cell(std::uintmax_t, cell&);
    bool get_cells(std::uintmax_t, std::uintmax_t, std::vector<cell>&);
    void list_cell(std::uintmax_t c1, std::uintmax_t c2);
    void list_cell(std::uintmax_t c);
    void list_cell();
  };
}

#endif // READCSV_H

