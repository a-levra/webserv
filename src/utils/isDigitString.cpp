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
	return (std::find_if_not(s.begin(), s.end(), isdigit) != s.end());
}