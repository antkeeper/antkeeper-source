// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GENETICS_SEQUENCE_HPP
#define ANTKEEPER_GENETICS_SEQUENCE_HPP

#include <engine/genetics/base.hpp>
#include <engine/genetics/codon.hpp>
#include <algorithm>
#include <iterator>
#include <random>

namespace genetics {

/// Functions and structures related to sequences of IUPAC degenerate base symbols.
namespace sequence {

/// Open reading frame (ORF), defined by a start codon and stop codon, with the distance between divisible by three.
/// @tparam Iterator Sequence iterator type.
template <class Iterator>
struct orf
{
	/// Iterator to the first base of the start codon.
	Iterator start;
	
	/// Iterator to the first base of the stop codon.
	Iterator stop;
};

/// Exchanges elements between two ranges, starting at a random offset.
/// @param first1,last1 First range of elements to crossover.
/// @param first2 Beginning of the second range of elements to crossover.
/// @param g Uniform random bit generator.
/// @return Iterator to the start of the crossover in the second range.
template <class ForwardIt1, class ForwardIt2, class URBG>
ForwardIt2 crossover(ForwardIt1 first1, ForwardIt1 last1, ForwardIt2 first2, URBG&& g);

/// Exchanges elements between two ranges multiple times, starting at a random offset each time.
/// @param first1,last1 First range of elements to crossover.
/// @param first2 Beginning of the second range of elements to crossover.
/// @param count Number of times to crossover.
/// @param g Uniform random bit generator.
template <class ForwardIt1, class ForwardIt2, class Size, class URBG>
void crossover_n(ForwardIt1 first1, ForwardIt1 last1, ForwardIt2 first2, Size count, URBG&& g);

/// Searches a sequence for an open reading frame (ORF).
/// @param first,last Range of elements to search.
/// @param table Genetic code translation table.
/// @return First ORF in the sequence, or `{last, last}` if no ORF was found.
template <class ForwardIt>
orf<ForwardIt> find_orf(ForwardIt first, ForwardIt last, const codon::table& table);

/// Applies the given function to a randomly selected element in a range.
/// @param first,last Range of elements to mutate.
/// @param unary_op Unary operation function object that will be applied.
/// @param g Uniform random bit generator.
/// @return Iterator to the mutated element.
template <class ForwardIt, class UnaryOperation, class URBG>
ForwardIt mutate(ForwardIt first, ForwardIt last, UnaryOperation unary_op, URBG&& g);

/// Applies the given function to a random selection of elements in a range.
/// @param first,last Range of elements to mutate.
/// @param count Number of elements to mutate.
/// @param unary_op Unary operation function object that will be applied.
/// @param g Uniform random bit generator.
template <class ForwardIt, class Size, class UnaryOperation, class URBG>
void mutate_n(ForwardIt first, ForwardIt last, Size count, UnaryOperation unary_op, URBG&& g);

/// Searches a sequence of IUPAC base symbols for a pattern matching a search string of IUPAC degenerate base symbols.
/// @param first,last Sequence of IUPAC base symbols to search.
/// @param s_first,s_last Search string of IUPAC degenerate base symbols.
/// @param stride Distance between consecutive searches.
/// @return Iterator to the beginning of the first subsequence matching `[s_first, s_last)` in the sequence `[first, last)`. If no such occurrence is found, @p last is returned.
template <class ForwardIt1, class ForwardIt2>
ForwardIt1 search(ForwardIt1 first, ForwardIt1 last, ForwardIt2 s_first, ForwardIt2 s_last, typename std::iterator_traits<ForwardIt1>::difference_type stride);

/// Transcribes a sequence of IUPAC base symbols between DNA and RNA, swapping `T` for `U` or `U` for `T`.
/// @param first,last Range of elements to transcribe.
/// @param d_first Beginning of the destination range.
/// @return Output iterator to the element past the last element transcribed. 
template <class InputIt, class OutputIt>
OutputIt transcribe(InputIt first, InputIt last, OutputIt d_first);

/// Translates a sequence of codons into amino acids.
/// @param first,last Open reading frame.
/// @param d_first Beginning of destination range.
/// @param table Genetic code translation table.
/// @return Output iterator to the element past the last element translated. 
template <class InputIt, class OutputIt>
OutputIt translate(InputIt first, InputIt last, OutputIt d_first, const codon::table& table);

/// Functions which operate on sequences of IUPAC degenerate **DNA** base symbols.
namespace dna
{
	/// Generates the complementary sequence for a sequence of IUPAC degenerate DNA base symbols.
	/// @param first,last Range of elements to complement.
	/// @param d_first Beginning of the destination range.
	/// @return Output iterator to the element past the last element complemented. 
	template <class InputIt, class OutputIt>
	OutputIt complement(InputIt first, InputIt last, OutputIt d_first);
}

/// Functions which operate on sequences of IUPAC degenerate **RNA** base symbols.
namespace rna
{
	/// Generates the complementary sequence for a sequence of IUPAC degenerate RNA base symbols.
	/// @param first,last Range of elements to complement.
	/// @param d_first Beginning of the destination range.
	/// @return Output iterator to the element past the last element complemented. 
	template <class InputIt, class OutputIt>
	OutputIt complement(InputIt first, InputIt last, OutputIt d_first);
}

template <class ForwardIt1, class ForwardIt2, class URBG>
ForwardIt2 crossover(ForwardIt1 first1, ForwardIt1 last1, ForwardIt2 first2, URBG&& g)
{
	typedef typename std::iterator_traits<ForwardIt1>::difference_type difference_t;
	std::uniform_int_distribution<difference_t> distribution(0, std::distance(first1, last1) - 1);
	difference_t pos = distribution(g);
	std::advance(first1, pos);
	std::advance(first2, pos);
	std::swap_ranges(first1, last1, first2);
	return first2;
}

template <class ForwardIt1, class ForwardIt2, class Size, class URBG>
void crossover_n(ForwardIt1 first1, ForwardIt1 last1, ForwardIt2 first2, Size count, URBG&& g)
{
	typedef typename std::iterator_traits<ForwardIt1>::difference_type difference_t;
	
	std::uniform_int_distribution<difference_t> distribution(0, std::distance(first1, last1) - 1);
	ForwardIt1 crossover1, crossover2;
	
	while (count)
	{
		crossover1 = first1;
		crossover2 = first2;
		
		difference_t pos = distribution(g);
		std::advance(crossover1, pos);
		std::advance(crossover2, pos);
		std::swap_ranges(crossover1, last1, crossover2);
		
		--count;
	}
}

template <class ForwardIt>
orf<ForwardIt> find_orf(ForwardIt first, ForwardIt last, const codon::table& table)
{
	ForwardIt second;
	ForwardIt third;
	orf<ForwardIt> result;
	
	auto distance = std::distance(first, last);
	
	if (distance >= 3)
	{
		second = first;
		++second;
		third = second;
		++third;
		
		do
		{
			if (codon::is_start(*first, *second, *third, table.starts))
			{
				result.start = first;
				distance -= 3;
				break;
			}
			
			first = second;
			second = third;
			++third;
			--distance;
		}
		while (third != last);
	}
	
	for (; distance >= 3; distance -= 3)
	{
		first = ++third;
		second = ++third;
		++third;
		
		if (codon::is_stop(*first, *second, *third, table.aas))
		{
			result.stop = first;
			return result;
		}
	}
	
	return {last, last};
}

template <class ForwardIt, class UnaryOperation, class URBG>
ForwardIt mutate(ForwardIt first, ForwardIt last, UnaryOperation unary_op, URBG&& g)
{
	typedef typename std::iterator_traits<ForwardIt>::difference_type difference_t;
	
	if (first == last)
		return first;
	
	std::uniform_int_distribution<difference_t> distribution(0, std::distance(first, last) - 1);
	std::advance(first, distribution(g));
	*first = unary_op(*first);
	
	return first;
}

template <class ForwardIt, class Size, class UnaryOperation, class URBG>
void mutate_n(ForwardIt first, ForwardIt last, Size count, UnaryOperation unary_op, URBG&& g)
{
	typedef typename std::iterator_traits<ForwardIt>::difference_type difference_t;
	
	if (first == last)
		return first;
	
	std::uniform_int_distribution<difference_t> distribution(0, std::distance(first, last) - 1);
	ForwardIt mutation;
	
	while (count)
	{
		mutation = first;
		std::advance(mutation, distribution(g));
		*mutation = unary_op(*mutation);
		--count;
	}
}

template <class ForwardIt1, class ForwardIt2>
ForwardIt1 search(ForwardIt1 first, ForwardIt1 last, ForwardIt2 s_first, ForwardIt2 s_last, typename std::iterator_traits<ForwardIt1>::difference_type stride)
{
	for (auto distance = std::distance(first, last); distance > 0; distance -= stride)
	{
		ForwardIt1 it = first;
		for (ForwardIt2 s_it = s_first; ; ++it, ++s_it)
		{
			if (s_it == s_last)
				return first;
			
			if (it == last)
				return last;
			
			if (!base::compare(*it, *s_it))
				break;
		}
		
		if (distance > stride)
			std::advance(first, stride);
	}
	
	return last;
}

template <class InputIt, class OutputIt>
inline OutputIt transcribe(InputIt first, InputIt last, OutputIt d_first)
{
	return std::transform(first, last, d_first, base::transcribe);
}

template <class InputIt, class OutputIt>
OutputIt translate(InputIt first, InputIt last, OutputIt d_first, const codon::table& table)
{
	auto length = std::distance(first, last);
	
	if (length >= 3)
	{
		InputIt second = first;
		++second;
		InputIt third = second;
		++third;
		
		*(d_first++) = codon::translate(*first, *second, *third, table.starts);
		
		for (length -= 3; length >= 3; length -= 3)
		{
			first = ++third;
			second = ++third;
			++third;
			
			*(d_first++) = codon::translate(*first, *second, *third, table.aas);
		}
	}
	
	return d_first;
}

namespace dna
{
	template <class InputIt, class OutputIt>
	inline OutputIt complement(InputIt first, InputIt last, OutputIt d_first)
	{
		return std::transform(first, last, d_first, base::dna::complement);
	}
}

namespace rna
{
	template <class InputIt, class OutputIt>
	inline OutputIt complement(InputIt first, InputIt last, OutputIt d_first)
	{
		return std::transform(first, last, d_first, base::rna::complement);
	}
}

} // namespace sequence
} // namespace genetics

#endif // ANTKEEPER_GENETICS_SEQUENCE_HPP
