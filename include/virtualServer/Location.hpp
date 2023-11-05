#ifndef LOCATION_HPP
# define LOCATION_HPP

#include <string>
#include <vector>
#include <map>

class Location {
	public:
		Location();
		Location(const std::string& URI);
		Location(const Location &other);
		virtual ~Location();

		Location &operator=(const Location &other);
		//getters and setters

		void	addDirective(const std::string& name, const std::string& content);
		void	addIndexDirective(const std::string& content);
		void	addAllowMethodsDirective(const std::string& content);
		void	addErrorPageDirective(const std::string& content);
		void	addReturnDirective(const std::string& content);
		void	addCGIPathDirective(const std::string& content);

		std::string getURI() const;
		std::string getRoot() const;
		std::string getAlias() const;
		size_t	getClientMaxBodySize() const;
		bool getAutoIndex() const;
		const std::vector<std::string>& getIndex() const;
		const std::vector<std::string>& getAllowMethods() const;
		const std::pair<std::vector<int>, std::string>& getErrorPage() const;
		const std::pair<int, std::string>& getReturn() const;
		const std::map<std::string, std::string>& getCGIPath() const;

		void	setURI(const std::string& URI);
		void	setRoot(const std::string& root);
		void	setClientMaxBodySize(size_t clientMaxBodySize);
		void	setAutoIndex(bool autoIndex);
		void	setIndex(const std::vector<std::string>& index);
		void	setAllowMethods(const std::vector<std::string>& allowMethods);
		void	setErrorPage(const std::pair<std::vector<int>, std::string>& errorPage);
		void	setReturn(const std::pair<int, std::string>& returnPage);
		bool	isAllowedMethod(const std::string& method) const;
		bool	hasCGI() const;
		bool	hasReturn() const;
		void display();

	private:
		std::string _URI;
		std::string _root;
		std::vector<std::string> _index;
		std::vector<std::string> _allowMethods;
		std::pair<std::vector<int>, std::string> _errorPage;
		std::pair<int, std::string> _return;
		std::map<std::string, std::string> _cgiPath;
		size_t _clientMaxBodySize;
		bool _autoIndex;
		bool _hasReturn;
};

#endif