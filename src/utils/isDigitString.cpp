/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   isDigitString.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tdameros <tdameros@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/11 11:58:00 by tdameros          #+#    #+#             */
/*   Updated: 2023/10/11 11:58:00 by tdameros         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include <algorithm>
#include <string>

bool	isDigitString(const std::string& s) {
	return (std::find_if(s.begin(), s.end(),
						 std::not1(std::ptr_fun(isdigit))) != s.end());
}