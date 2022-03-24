/*
#include "RsBoostType.h"
*/

#ifndef _RsBoostType_INCLUDED__
#define _RsBoostType_INCLUDED__

#pragma warning (push, 3)

#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include "boost/format.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/scoped_ptr.hpp"

#pragma region BOOST types

  typedef boost::tokenizer<boost::char_separator<char> >	rsTokenizer;
  typedef rsTokenizer::iterator								rsTokenizer_iter;
  typedef boost::char_separator<char>						rsCharSeparator;


#pragma endregion BOOST types


#pragma warning (pop)

#endif //_RsBoostType_INCLUDED__