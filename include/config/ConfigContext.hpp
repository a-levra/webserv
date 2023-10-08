#ifndef CONFIGCONTEXT_HPP
#define CONFIGCONTEXT_HPP

#include <string>
#include <vector>
#include <map>

class Context {
public:
	Context();
	Context(const std::string& name, const std::string& arguments, const std::string &content);

	void	addDirective(const std::string& name, const std::string& content);
	void	addContext(Context context);

	void	inheritDirectives(void);

	std::string getName() const;
	std::string getContent() const;

	const std::vector<Context>&	getContexts() const;

private:
	std::string							_name;
	std::string							_arguments;
	std::string 						_content;
	std::vector<Context>				_subContexts;
	std::map<std::string, std::string>	_directives;
};

#endif
