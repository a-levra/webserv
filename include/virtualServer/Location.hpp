#ifndef LOCATION_HPP
# define LOCATION_HPP

#include <string>
class location {
	public:
		location();
		location(const location &other);
		virtual ~location();

		location &operator=(const location &other);
		//getters and setters
		std::string getRoot() const;
		void setRoot(const std::string &root);
		std::string getIndex() const;
		void setIndex(const std::string &index);
		size_t getClientMaxBodySize() const;
		void setClientMaxBodySize(size_t clientMaxBodySize);
		bool isAutoindex() const;
		void setAutoindex(bool autoindex);

	private:
		std::string _root;
		std::string _index;
		//allow method
		size_t _clientMaxBodySize;
		bool _autoindex;
};

#endif