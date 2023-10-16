#ifndef LOCATION_HPP
# define LOCATION_HPP
#include <vector>
#include <string>
class Location {
	public:
		Location();
		Location(const Location &other);
		virtual ~Location();

		Location &operator=(const Location &other);
		//getters and setters
		std::string getRoot() const;
		void setRoot(const std::string &root);
		std::string getIndex() const;
		void setIndex(const std::string &index);
		size_t getClientMaxBodySize() const;
		void setClientMaxBodySize(size_t clientMaxBodySize);
		bool isAutoindex() const;
		void setAutoindex(bool autoindex);
		std::vector<std::string> & getMethods();
		void addMethod(const std::string &method);
		void display();
		void setUri(const char *string);
		std::string getUri();
	protected:
		std::string _uri;
		std::string _root;
		std::string _index;
		size_t _clientMaxBodySize;
		std::vector<std::string> _allowedMethods;
		bool _autoindex;
};

#endif