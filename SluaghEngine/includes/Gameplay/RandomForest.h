#ifndef SE_GAMEPLAY_RANDOM_FOREST_H
#define SE_GAMEPLAY_RANDOM_FOREST_H

#include "DecisionTree.h"
#include <tuple>
#include <Profiler.h>

namespace SE
{
	namespace Gameplay
	{
		namespace DecisionTree
		{
			template<typename ...Args>
			class RandomForest
			{
			private:
				typedef std::tuple<std::vector<DecisionTree<Args>*>...> Forest;
				typedef std::tuple<std::vector<Args>...> AnswerTypes;
				typedef std::vector<std::vector<IFeature*>> TrainingData;

				Forest treesInTheForest;
				AnswerTypes answerVectors;
				TrainingData trainingData;

				std::vector<int> numberOfPossibleAnswersForEachTree;

				static const int numberOfTreeTypes = sizeof...(Args);

				int numberOfSamplesBeforeNewTree = 1500;

				/*
				 * Code to get out one vector from the tuple, as described here:
				 * https://stackoverflow.com/questions/27941661/generating-one-class-member-per-variadic-template-argument
				 */

#pragma region Black Magic
				template <class T1, class T2>
				struct SameType
				{
					static const bool value = false;
				};

				template<class T>
				struct SameType<T, T>
				{
					static const bool value = true;
				};

				template<int N, typename T>
				struct VectorOfForest : SameType<T,
					typename std::tuple_element<N, Forest>::type::value_type>
				{ };

				template <int N, class T, class Tuple, bool Match = false> // this =false is only for clarity
				struct MatchingForest
				{
					static std::vector<DecisionTree<T>*>& get(Tuple& tp)
					{
						// The "non-matching" version
						return MatchingForest<N + 1, T, Tuple,
							VectorOfForest<N + 1, DecisionTree<T>*>::value>::get(tp);
					}
				};

				template <int N, class T, class Tuple>
				struct MatchingForest<N, T, Tuple, true>
				{
					static std::vector<DecisionTree<T>*>& get(Tuple& tp)
					{
						StartProfile;
						ProfileReturn(std::get<N>(tp));
					}
				};

				template<int N, typename T>
				struct VectorOfAnswer : SameType<T,
					typename std::tuple_element<N, AnswerTypes>::type::value_type>
				{ };

				template <int N, class T, class Tuple, bool Match = false> // this =false is only for clarity
				struct MatchingAnswer
				{
					static std::vector<T>& get(Tuple& tp)
					{
						// The "non-matching" version
						return MatchingAnswer<N + 1, T, Tuple,
							VectorOfAnswer<N + 1, T>::value>::get(tp);
					}
				};

				template <int N, class T, class Tuple>
				struct MatchingAnswer<N, T, Tuple, true>
				{
					static std::vector<T>& get(Tuple& tp)
					{
						StartProfile;
						ProfileReturn(std::get<N>(tp));
					}
				};

				template <typename T>
				void access_forest(std::vector<DecisionTree<T>*>& returnVector)
				{
					StartProfile;
					returnVector = MatchingForest<0, T, Forest,
						VectorOfForest<0, DecisionTree<T>*>::value>::get(treesInTheForest);
					StopProfile;
				}

				template<typename T>
				void AddAnswer(T answer)
				{
					StartProfile;
					MatchingAnswer<0, T, AnswerTypes,
						VectorOfAnswer<0, T>::value>::get(answerVectors).push_back(answer);
					StopProfile;
				}

				template<std::size_t I = 0, typename FuncT>
				inline typename std::enable_if<I == sizeof...(Args), void>::type
					for_each_forest(Forest &, FuncT) // Unused arguments are given no names.
				{

				}

				template<std::size_t I = 0, typename FuncT>
				inline typename std::enable_if < I < sizeof...(Args), void>::type
					for_each_forest(Forest& t, FuncT f)
				{
					StartProfile;
					f(std::get<I>(t));
					for_each_forest<I + 1, FuncT, Args...>(t, f);
					StopProfile;
				}

				template<std::size_t I = 0, typename FuncT>
				inline typename std::enable_if<I == sizeof...(Args), void>::type
					for_each_answer_type(AnswerTypes &, FuncT) // Unused arguments are given no names.
				{

				}

				template<std::size_t I = 0, typename FuncT>
				inline typename std::enable_if < I < sizeof...(Args), void>::type
					for_each_answer_type(AnswerTypes& t, FuncT f)
				{
					StartProfile;
					f(std::get<I>(t));
					for_each_answer_type<I + 1, FuncT, Args...>(t, f);
					StopProfile;
				}

				template<typename FirstAnswer, typename ...Answers>
				void AddMultipleAnswers(FirstAnswer answer, Answers ...answers)
				{
					StartProfile;
					AddAnswer(answer);
					AddMultipleAnswers(answers...);
					StopProfile;
				}

				template<typename FirstAnswer>
				void AddMultipleAnswers(FirstAnswer answer)
				{
					StartProfile;
					AddAnswer(answer);
					StopProfile;
				}

				void AddMultipleAnswers() {};

				template<std::size_t I = 0>
				inline typename std::enable_if<I == sizeof...(Args), void>::type
					create_new_tree_for_each_answer_type(AnswerTypes &) // Unused arguments are given no names.
				{

				}

				template<std::size_t I = 0>
				inline typename std::enable_if < I < sizeof...(Args), void>::type
					create_new_tree_for_each_answer_type(AnswerTypes& t)
				{
					StartProfile;
					CreateTree(std::get<I>(t), I);
					create_new_tree_for_each_answer_type<I + 1>(t);
					std::get<I>(t).clear();
					StopProfile;
				}

				template <typename T>
				void CreateTree(std::vector<T> data, std::size_t ID)
				{
					StartProfile;
					std::vector<typename DecisionTree<T>::FeatureList*> trainingDataForTree;
					unsigned int counter = 0;
					for (auto sampleAnswer : data)
						trainingDataForTree.push_back(new typename DecisionTree<T>::FeatureList(trainingData[counter++], sampleAnswer));


					MatchingForest<0, T, Forest,
						VectorOfForest<0, DecisionTree<T>*>::value>::get(treesInTheForest).push_back(
							new DecisionTree<T>(trainingDataForTree, numberOfPossibleAnswersForEachTree[ID])
						);

					for (auto featureList : trainingDataForTree)
						delete featureList;
					StopProfile;
				}

				void CreateNewTrees()
				{
					StartProfile;
					create_new_tree_for_each_answer_type(answerVectors);
					StopProfile;
				}

				template<std::size_t I = 0>
				inline typename std::enable_if<I == sizeof...(Args), void>::type
					delete_all_trees_for_each_answer_type(AnswerTypes &) // Unused arguments are given no names.
				{

				}

				template<std::size_t I = 0>
				inline typename std::enable_if < I < sizeof...(Args), void>::type
					delete_all_trees_for_each_answer_type(AnswerTypes& t)
				{
					StartProfile;
					DeleteTree(std::get<I>(t));
					delete_all_trees_for_each_answer_type<I + 1>(t);
					StopProfile;
				}

				template <typename T>
				void DeleteTree(std::vector<T> data)
				{
					StartProfile;
					auto forest = MatchingForest<0, T, Forest,
						VectorOfForest<0, DecisionTree<T>*>::value>::get(treesInTheForest);
					for (auto tree : forest)
						delete tree;
					StopProfile;
				}
				void DeleteAllTrees()
				{
					StartProfile;
					delete_all_trees_for_each_answer_type(answerVectors);
					StopProfile;
				}

				template <int N, class T, class Tuple, bool Match = false> // this =false is only for clarity
				struct MatchingID
				{
					static int get(Tuple& tp)
					{
						// The "non-matching" version
						return MatchingID<N + 1, T, Tuple,
							VectorOfAnswer<N + 1, T>::value>::get(tp);
					}
				};

				template <int N, class T, class Tuple>
				struct MatchingID<N, T, Tuple, true>
				{
					static int get(Tuple& tp)
					{
						return N;
					}
				};


				template<typename Answer>
				Answer AskTheForest(std::vector<IFeature*> &toClassify)
				{
					StartProfile;
					std::vector<Answer> votes;

					auto forest = MatchingForest<0, Answer, Forest,
						VectorOfForest<0, DecisionTree<Answer>*>::value>::get(treesInTheForest);

					for (auto tree : forest)
						votes.push_back(tree->Decide(toClassify));

					auto AnswerID = MatchingID<0, Answer, AnswerTypes,
						VectorOfAnswer<0, Answer>::value>::get(answerVectors);

					std::vector<int> numberOfVotes(numberOfPossibleAnswersForEachTree[AnswerID]);
					for (auto &answer : numberOfVotes)
						answer = 0;

					for (auto vote : votes)
					{
						++numberOfVotes[int(vote)];
					}

					Answer winner = static_cast<Answer>(0);
					int highestVote = 0;
					int counter = 0;
					for (auto amount : numberOfVotes)
					{
						if (highestVote < amount)
						{
							highestVote = amount;
							winner = static_cast<Answer>(counter);
						}
						counter++;
					}

					ProfileReturnConst(winner);
				}



#pragma endregion



			public:
				/*RandomForest() : numberOfSamplesBeforeNewTree(samplesBeforeNewTree) {};*/
				RandomForest()
				{

				}
				~RandomForest()
				{
					
					DeleteAllTrees();
				}

				void SetNumberOfPossibleAnswersForDecisionTrees(std::vector<int> numberOfPossibleAnswers)
				{
					StartProfile;
					numberOfPossibleAnswersForEachTree = numberOfPossibleAnswers;
					StopProfile;
				}


				template<typename FirstAnswer, typename ...Answers>
				void AddTrainingExample(std::vector<IFeature*> trainingExample, FirstAnswer answer, Answers ...answers)
				{
					StartProfile;
					trainingData.push_back(trainingExample);
					AddAnswer(answer);
					AddMultipleAnswers(answers...);

					if (trainingData.size() == numberOfSamplesBeforeNewTree)
					{
						CreateNewTrees();
						trainingData.clear();
					}
					StopProfile;
				}

				template<typename Answer>
				void GetAnswer(std::vector<IFeature*> &toClassify, Answer &answer)
				{
					StartProfile;
					answer = AskTheForest<Answer>(toClassify);
					StopProfile;
				}
			};


		}
	}
}






#endif


