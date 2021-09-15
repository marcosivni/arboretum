/**********************************************************************
* GBDI Arboretum - Copyright (c) 2002-2004 GBDI-ICMC-USP
*
*                           Homepage: http://gbdi.icmc.usp.br/arboretum
**********************************************************************/
/* ====================================================================
 * The GBDI-ICMC-USP Software License Version 1.0
 *
 * Copyright (c) 2004 Grupo de Bases de Dados e Imagens, Instituto de
 * CiÃªncias MatemÃ¡ticas e de ComputaÃ§Ã£o, University of SÃ£o Paulo -
 * Brazil (the Databases and Image Group - Intitute of Matematical and 
 * Computer Sciences).  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The end-user documentation included with the redistribution,
 *    if any, must include the following acknowledgment:
 *       "This product includes software developed by Grupo de Bases
 *        de Dados e Imagens, Instituto de CiÃªncias MatemÃ¡ticas e de
 *        ComputaÃ§Ã£o, University of SÃ£o Paulo - Brazil (the Databases 
 *        and Image Group - Intitute of Matematical and Computer 
 *        Sciences)"
 *
 *    Alternately, this acknowledgment may appear in the software itself,
 *    if and wherever such third-party acknowledgments normally appear.
 *
 * 4. The names of the research group, institute, university, authors
 *    and collaborators must not be used to endorse or promote products
 *    derived from this software without prior written permission.
 *
 * 5. The names of products derived from this software may not contain
 *    the name of research group, institute or university, without prior
 *    written permission of the authors of this software.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS OF THIS SOFTWARE OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * ====================================================================
 *                                            http://gbdi.icmc.usp.br/
 */
#ifndef __STDUMMYTREE_H
#define __STDUMMYTREE_H

#include <math.h>
#include <arboretum/stCommon.h>
#include <arboretum/stMetricTree.h>
#include <arboretum/stDummyNode.h>

#include <exception>
#include <iostream>
#include <ctime>


/**
* This class template implements a Dummy Tree. The Dummy Tree has the same
* external interface and behavior of a Metric Tree but is implemented as
* sequential list.
*
* <P>It can perform both range and k-nearest neighbout queries but without
* the performance associated with other structures. Its algorithms always
* have O(N) (except for insertion which is O(1)). In other words, it will
* take a lot of time to give answers but it will assure their correctness.
*
* <P>This class was developed to generate perfect answers to queries. It
* allows the build of automated test programs for other metric trees
* implemented by this library.
*
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @todo Finish the implementation.
* @todo Compute statistics.
* @version 1.0
* @ingroup dummy
*/
template <class ObjectType, class EvaluatorType>
class stDummyTree: public stMetricTree<ObjectType, EvaluatorType>{

   public:      

      /**
      * This is the class that abstracts the object used by this metric tree.
      */
      typedef ObjectType tObject;

      /**
      * This is the class that abstracts the metric evaluator used by this metric
      * tree.
      */
      typedef EvaluatorType tMetricEvaluator;

      /**
      * This is the class that abstracts an result set for simple queries.
      */
      typedef stResult <ObjectType> tResult;
#ifdef __stCKNNQ__
      /**
      * This is the class that abstracts an result set for simple queries.
      */
      typedef stConstrainedResult <ObjectType> tConstrainedResult;
      
#endif // __stCKNNQ__
      /**
      * This is the class that abstracts an result set for joined queries.
      */
      typedef stJoinedResult <ObjectType> tJoinedResult;


     class Iterator{
     public:
       Iterator(){
         pageID_ = 0;
         entryOnPage_ = 0;
         pageManager_ = NULL;
         currPage = NULL;
         currNode = NULL;
         numberOfEntries_ = 0;
         obj_ = new tObject();
       }

       Iterator(u_int32_t pageID, int entryOnPage, stPageManager* pageManager){
         currPage = NULL;
         currNode = NULL;
         numberOfEntries_ = 0;
         obj_ = new tObject();

         pageID_ = pageID;
         entryOnPage_ = entryOnPage;
         pageManager_ = pageManager;

         load();
       }

       virtual ~Iterator(){
         if(currNode)
           delete currNode;
         if(currPage)
           pageManager_->ReleasePage(currPage);
         if(obj_)
           delete obj_;
       }


       u_int32_t getPageID(){
         return pageID_;
       }

       int entryOnPage(){
         return entryOnPage_;
       }

       tObject* getObject(){
         return obj_;
       }

       tObject* operator*(){
         return getObject();
       }


       Iterator& operator++(int){
         entryOnPage_++;
         if(entryOnPage_ < numberOfEntries_){
           loadObject();
         }else{

           pageID_ = currNode->GetNextNode();
           entryOnPage_ = 0;

           delete currNode;
           pageManager_->ReleasePage(currPage);
           currNode = NULL;
           currPage = NULL;

           load();
         }
       }

       bool operator==(const Iterator& it){
         if(pageID_ == it.pageID_ && entryOnPage_ == it.entryOnPage_)
           return true;
         else
           return false;
       }

       bool operator!=(const Iterator& it){
         if(pageID_ == it.pageID_ && entryOnPage_ == it.entryOnPage_)
           return false;
         else
           return true;
       }


       Iterator& operator=(const Iterator& it){
         if(currNode)
           delete currNode;
         if(currPage)
           pageManager_->ReleasePage(currPage);

         currPage = NULL;
         currNode = NULL;
         numberOfEntries_ = 0;

         pageID_ = it.pageID_;
         entryOnPage_ = it.entryOnPage_;
         pageManager_ = it.pageManager_;

         load();
       }
     private:
       bool load(){
         if(pageID_ != 0){

           currPage = pageManager_->GetPage(pageID_);
           currNode = new stDummyNode(currPage);

           numberOfEntries_ = currNode->GetNumberOfEntries();

           return loadObject();
         }
         return false;
       }


       bool loadObject(){
         if(entryOnPage_ < numberOfEntries_){
           obj_->Unserialize(currNode->GetObject(entryOnPage_), currNode->GetObjectSize(entryOnPage_));
           return true;
         }else
           throw;
         return false;
       }

       u_int32_t pageID_;
       int entryOnPage_;
       stPageManager* pageManager_;
       tObject* obj_;


       stPage* currPage;
       stDummyNode* currNode;
       int numberOfEntries_;
     };

     Iterator begin(){
       u_int32_t pageID;
       int entryOnPage;

       pageID = this->GetRoot();
       entryOnPage = 0;
       return Iterator(pageID, entryOnPage, this->myPageManager);
     }

     Iterator end(){
       return Iterator(0, 0, this->myPageManager);
     }





      /**
      * Creates a new instance of the Dummy Tree.
      *
      * @param pageman The Page Manager to be used.
      */
      stDummyTree(stPageManager * pageman);


      stDummyTree(stPageManager * pageman, EvaluatorType* metricEvaluator);

      /**
      * Disposes this tree and releases all associated resources.
      */
      virtual ~stDummyTree(){

         if (HeaderPage != NULL){
            // Release the header page.
            this->myPageManager->ReleasePage(HeaderPage);
         }//end if
      }//end ~stDummyTree

      /**
      * This method adds an object to the metric tree. This method may fail it the object size
      * exceeds the page size - 16.
      *
      * @param obj The object to be added.
      * @return True for success or false otherwise.
      */
      virtual bool Add(tObject * obj);

      /**
      * Returns the number of objetcs of this sequencial scan.
      */
      virtual long GetNumberOfObjects(){
         return Header->ObjectCount;
      }//end GetNumberOfObjects

      /**
      * Returns the number of nodes of this tree.
      */
      virtual long GetNodeCount(){
         return Header->NodeCount;
      }//end GetNodeCount

      /**
      * Returns the MaxOccupation of the nodes.
      */
      virtual u_int32_t GetMaxOccupation(){
         return Header->MaxOccupation;
      }//end GetMaxOccupation

      /**
      * Returns the MaxOccupation of the nodes.
      */
      void SetMaxOccupation(u_int32_t newValue){
         if (newValue > Header->MaxOccupation){
            Header->MaxOccupation = newValue;
         }//end if
      }//end SetMaxOccupation

      /**
      * Returns the root pageID.
      */
      u_int32_t GetRoot(){
         return Header->Root;
      }//end GetRoot


      std::vector < stResult<ObjectType> * > diverseBRIDGEQuery(tObject * sample, u_int32_t k){

          tResult *globalNN;
          tResult *bridgeList;
          std::vector < stResult<ObjectType> * > answer;

          //Sort candidates
          globalNN = new tResult();
          globalNN->SetQueryInfo(sample->Clone(), RANGEQUERY, -1, DBL_MAX, false);
          globalNN = this->RangeQuery(sample->Clone(), DBL_MAX); //sorted list by distance
          //Stops if dataset is empty
          if (!globalNN->GetNumOfEntries()){
              return answer;
          }

          //Insert the closest element in the result set and creates a list of influenced elements for it
          bridgeList = new tResult();
          bridgeList->SetQueryInfo(sample->Clone(), KNEARESTQUERY, 0, 0.0, false);
          bridgeList->AddPair( ((ObjectType *) (*globalNN)[0].GetObject())->Clone(), (*globalNN)[0].GetDistance() );
          //std::cout << "cabeca da lista " <<  ((ObjectType *) (*globalNN)[0].GetObject())->getOID() << "-" << (*globalNN)[0].GetDistance() << std::endl;
          answer.push_back(bridgeList);

          //Examine every other (sorted) candidate
          for (size_t j = 1; j < globalNN->GetNumOfEntries(); j++){
              double dOjOq = this->myMetricEvaluator->GetDistance(sample, (ObjectType *) (*globalNN)[j].GetObject());

              size_t closestInfluent = 0;
              double minDistToInfluent = DBL_MAX;
              bool influenced = false;

              //Build the strong influence sets
              for (size_t i = 0; i < answer.size(); i++){
//                  std::cout << "influente " <<
//                               ((ObjectType *) (*answer.at(i))[0].GetObject())->getOID()  << " candidato " <<
//                               ((ObjectType *) (*globalNN)[j].GetObject())->getOID() << "-" << std::endl;
                  double dOiOj =  this->myMetricEvaluator->GetDistance((ObjectType *) (*answer.at(i))[0].GetObject(), (ObjectType *) (*globalNN)[j].GetObject());
                  double dOiOq = this->myMetricEvaluator->GetDistance(sample, (ObjectType *) (ObjectType *) (*answer.at(i))[0].GetObject());
                  //std::cout << "dOjOq " << dOjOq << " dOiOj " << dOiOj << " dOiOq " << dOiOq << std::endl;
                  if ( ((1/dOiOj) >= (1/dOiOq)) && ((1/dOiOj) >= (1/dOjOq))){ //Second condition for sanity check
                      influenced = true;
                      if (minDistToInfluent > dOiOj){
                          minDistToInfluent = dOiOj;
                          closestInfluent = i;
                      }
                  }
              }

              if (!influenced){
                  //std::cout << "novo ifluente!" << ((ObjectType *) (*globalNN)[j].GetObject())->getOID() << "-" << std::endl;
                  if (answer.size() == k){
                      //Force break
                      break;
                  } else {
                      bridgeList = new tResult();
                      bridgeList->SetQueryInfo(sample->Clone(), KNEARESTQUERY, 0, 0.0, false);
                      bridgeList->AddPair( ((ObjectType *) (*globalNN)[j].GetObject())->Clone(), dOjOq );
                      answer.push_back(bridgeList);
                  }
              } else {
                  //std::cout << "novo influenciado!" << ((ObjectType *) (*globalNN)[j].GetObject())->getOID() << "para influente "
                  //          <<  ((ObjectType *) (*answer.at(closestInfluent))[0].GetObject())->getOID() <<  "-" << std::endl;
                  bridgeList = answer[closestInfluent];
                  bridgeList->SetQueryInfo(sample->Clone(), KNEARESTQUERY, 0, 0.0, false);
                  bridgeList->AddPair( ((ObjectType *) (*globalNN)[j].GetObject())->Clone(), minDistToInfluent+dOjOq);
              }
          }
          return answer;
      }

      /**
      * This method will perform a range query. The result will be a set of
      * pairs object/distance.
      *
      * @param sample The sample object.
      * @param range The range of the results.
      * @return The result or NULL if this method is not implemented.
      * @warning The instance of tResult returned must be destroied by user.
      */
      virtual tResult * RangeQuery(tObject * sample, double range);

      /**
       * This method gathers the numFoci first Omni-Foci depending od distance distribution.
       * See Traina2007 - OmniFamily for further details
       *
       * @param numFoci The desired number of foci
       * @param randomSeed The seed object of the heuristic
       * @return The list of Omni-Foci
       */
      virtual tResult * GetOmniFoci(u_int16_t numFoci);



      /**
      * This method will perform a reverse range query.
      * The result will be a set of pairs object/distance.
      *
      * @param sample The sample object.
      * @param range The range of the results. All object that are
      *  greater than the range distance will be included in the result set.
      * @return The result or NULL if this method is not implemented.
      * @warning The instance of tResult returned must be destroied by user.
      */
      virtual tResult * ReversedRangeQuery(tObject * sample, double range);


      /**
      * This method will perform a k nearest neighbor query.
      *
      * @param sample The sample object.
      * @param k The number of neighbours.
      * @param tie The tie list. Default false.
      * @return The result or NULL if this method is not implemented.
      * @warning The instance of tResult returned must be destroied by user.
      */
      virtual tResult * NearestQuery(tObject * sample, u_int32_t k, bool tie = false);

      /**
      * This method will perform a K-Farthest neighbor query.
      *
      * @param sample The sample object.
      * @param k The number of neighbours.
      * @param tie The tie list. Default false.
      * @return The result or NULL if this method is not implemented.
      * @warning The instance of tResult returned must be destroied by user.
      */
      virtual tResult * FarthestQuery(tObject * sample, u_int32_t k, bool tie = false);

      /**
      * This method will perform a range query with a limited number of results.
      *
      * <P>This query is a combination of the standard range query and the standard
      * k-nearest neighbour query. All objects which matches both conditions
      * will be included in the result.
      *
      * @param sample The sample object.
      * @param range The range of the results.
      * @param k The maximum number of results.
      * @param tie The tie list. This parameter is optional. Default false;
      * @return The result or NULL if this method is not implemented.
      * @warning The instance of tResult returned must be destroied by user.
      */
      virtual tResult * KAndRangeQuery(tObject * sample, double range,
            u_int32_t k, bool tie=false);

      /**
      * This method will perform range query with a limited number of results.
      *
      * <P>This query is a combination of the standard range query and the
      * standard k-nearest neighbour query. All objects which matches with
      * one of two conditions will be included in the result.
      *
      * @param sample The sample object.
      * @param range The range of the results.
      * @param k The maximum number of results.
      * @param tie The tie list. This parameter is optional. Default false;
      * @return The result or NULL if this method is not implemented.
      * @warning The instance of tResult returned must be destroied by user.
      */
      virtual tResult * KOrRangeQuery(tObject * sample, double range,
            u_int32_t k, bool tie=false);

      /**
      * This method will perform a ring query. The result will be a set of
      * pairs object/distance.
      *
      * <P>The object pointed by <b>sample</b> will not be destroyed by this
      * method.
      *
      * @param sample The sample object.
      * @param inRange The inner range of the results.
      * @param outRange The outter range of the results.
      * @return The result.
      * @warning The instance of tResult returned must be destroied by user.
      * @warning The inRange must be less than the outRange.
      * @exception std::logic_error If this method is not supported by this tree.
      */
      virtual tResult * RingQuery(tObject * sample, double inRange,
            double outRange);

      /**
      * This method will perform a grouping SUM range query. The result will be a set of
      * pairs object/distance.
      *
      * @param sampleList The list of sample objects.
      * @param sampleSize The number of samples of the param sampleList.
      * @param range The range of the results.
      * @return The result or NULL if this method is not implemented.
      * @warning The instance of tResult returned must be destroied by user.
      * @autor Implemented by Humberto Razente
      */
      tResult * SumRangeQuery(tObject ** sampleList, u_int32_t sampleSize,
                              double range);

      /**
      * This method will perform a grouping SUM k nearest neighbor query.
      *
      * @param sampleList The list of sample objects.
      * @param sampleSize The number of samples of the param sampleList.
      * @param k The number of neighbours.
      * @param tie The tie list. Default false.
      * @return The result or NULL if this method is not implemented.
      * @warning The instance of tResult returned must be destroied by user.
      */
      tResult * SumNearestQuery(tObject ** sampleList, u_int32_t sampleSize,
                                u_int32_t k, bool tie = false);

      /**
      * This method will perform a grouping MAX range query. The result will be a set of
      * pairs object/distance.
      *
      * @param sampleList The list of sample objects.
      * @param sampleSize The number of samples of the param sampleList.
      * @param range The range of the results.
      * @return The result or NULL if this method is not implemented.
      * @warning The instance of tResult returned must be destroied by user.
      * @autor Implemented by Humberto Razente
      */
      tResult * MaxRangeQuery(tObject ** sampleList, u_int32_t sampleSize,
                              double range);

      /**
      * This method will perform a grouping MAX k nearest neighbor query.
      *
      * @param sampleList The list of sample objects.
      * @param sampleSize The number of samples of the param sampleList.
      * @param k The number of neighbours.
      * @param tie The tie list. Default false.
      * @return The result or NULL if this method is not implemented.
      * @warning The instance of tResult returned must be destroied by user.
      */
      tResult* MaxNearestQuery(tObject ** sampleList, u_int32_t sampleSize,
                               u_int32_t k, bool tie = false);

      /**
      * This method will perform a grouping ALL range query. The result will be a set of
      * pairs object/distance.
      *
      * @param sampleList The list of sample objects.
      * @param sampleSize The number of samples of the param sampleList.
      * @param range The range of the results.
      * @return The result or NULL if this method is not implemented.
      * @warning The instance of tResult returned must be destroied by user.
      * @autor Implemented by Humberto Razente
      */
      tResult * AllRangeQuery(tObject ** sampleList, u_int32_t sampleSize,
                              double range);

      /**
      * This method will perform a grouping ALL k nearest neighbor query.
      *
      * @param sampleList The list of sample objects.
      * @param sampleSize The number of samples of the param sampleList.
      * @param k The number of neighbours.
      * @param tie The tie list. Default false.
      * @return The result or NULL if this method is not implemented.
      * @warning The instance of tResult returned must be destroied by user.
      */
      tResult* AllNearestQuery(tObject ** sampleList, u_int32_t sampleSize,
                               u_int32_t k, bool tie = false);

      /**
      * This method will perform a k-nearest neighbor joined query.
      *
      * @param dummyTree The Dummy tree to be joined.
      * @param k The number of neighbours.
      * @param tie The tie list. Default false.
      * @return The result or NULL if this method is not implemented.
      * @warning The instance of tJoinedResult returned must be destroied by user.
      * @autor Implemented by Humberto Razente
      */
      tJoinedResult * NearestJoinQuery(stDummyTree * dummyTree, u_int32_t k,
                                       bool tie = false);

      /**
      * This method will perform a range joined query.
      *
      * @param dummyTree The dummyTree to be joined.
      * @param range The range of the results.
      * @return The result or NULL if this method is not implemented.
      * @warning The instance of tJoinedResult returned must be destroied by user.
      * @autor Implemented by Humberto Razente
      */
      tJoinedResult * RangeJoinQuery(stDummyTree * dummyTree, double range);

      /**
      * This method will perform a k-closest neighbor joined query.
      *
      * @param dummyTree The Dummy tree to be joined.
      * @param k The number of neighbours.
      * @param tie The tie list. Default false.
      * @return The result or NULL if this method is not implemented.
      * @warning The instance of tJoinedResult returned must be destroied by user.
      * @autor Implemented by Humberto Razente
      */
      tJoinedResult * ClosestJoinQuery(stDummyTree * dummyTree, u_int32_t k,
                                       bool tie = false);

      /**
      * This method will perform a PRE-CONDITION CONSTRAINED k nearest neighbor query.
      *
      * @param sample The sample object.
      * @param k The number of neighbours.
      * @param tie The tie list. Default false.
      * @return The result or NULL if this method is not implemented.
      * @warning The instance of tResult returned must be destroied by user.
      */
      template <class TupleType>
      tResult * preConstrainedNearestQuery(tObject * sample, u_int32_t k, u_int32_t idx,
            bool (*compare)(const void *, const void *), const void * value);

      template <class TupleType>
      tResult * intraConstrainedNearestQueryCountGreaterThanOrEqual(tObject * sample,
            u_int32_t k, u_int32_t idx, bool (*compare)(const void *, const void *), const void * value,
            u_int32_t aggValue);

      template <class TupleType>
      tResult * intraConstrainedNearestQueryCountLessThanOrEqual(tObject * sample,
            u_int32_t k, u_int32_t idx, bool (*compare)(const void *, const void *), const void * value,
            u_int32_t aggValue);

      template <class TupleType>
      tResult * intraConstrainedNearestQueryCountDistinctGreaterThanOrEqual(tObject * sample,
            u_int32_t k, u_int32_t idx, bool (*compare)(const void *, const void *), const void * value,
            u_int32_t aggIdx, bool (*aggCompare)(const void *, const void *), u_int32_t aggValue);

      template <class TupleType>
      tResult * intraConstrainedNearestQueryCountDistinctLessThanOrEqual(tObject * sample,
            u_int32_t k, u_int32_t idx, bool (*compare)(const void *, const void *), const void * value,
            u_int32_t aggIdx, bool (*aggCompare)(const void *, const void *), u_int32_t aggValue);

   private:

      /**
      * This type defines the header of the Dummy Tree.
      */
      typedef struct DummyTreeHeader{
         /**
         * The root.
         */
         u_int32_t Root;

         /**
         * The number of the objects in this page.
         */
         u_int32_t ObjectCount;

         /**
         * Total number of nodes.
         */
         u_int32_t NodeCount;

         /**
         * Maximum number of objects in a node.
         */
         u_int32_t MaxOccupation;
      } stDummyTreeHeader;

      /**
      * The header page. It will be kept in memory all the time to avoid
      * reads.
      */
      stPage * HeaderPage;

      /**
      * The header of the "tree".
      */
      stDummyTreeHeader * Header;

      /**
      * If true, the header must be written to the page manager.
      */
      bool HeaderUpdate;

      /**
      * Creates the header for an empty tree.
      */
      void Create(){

         LoadHeader();
         Header->Root = 0;
         Header->ObjectCount = 0;
         Header->NodeCount = 0;
         Header->MaxOccupation = 0;
         HeaderUpdate = true;
         WriteHeader();
      }//end Create

      /**
      * Loads the header from the page manager.
      */
      void LoadHeader(){
         HeaderPage = this->myPageManager->GetHeaderPage();
         Header = (stDummyTreeHeader *) HeaderPage->GetData();
      }//end LoadHeader

      /**
      * Writes the header into the Page Manager.
      */
      void WriteHeader(){
         if (HeaderUpdate){
            this->myPageManager->WriteHeaderPage(HeaderPage);
            HeaderUpdate = false;
         }//end if
      }//end WriteHeader

      /**
      * Set the new root pageID.
      */
      void SetRoot(u_int32_t newRootPageID){
         Header->Root = newRootPageID;
      }//end SetRoot

      /**
      * Updates the object counter.
      */
      void UpdateObjectCounter(int inc){
         Header->ObjectCount += inc;
         HeaderUpdate = true;
      }//end UpdateObjectCounter

};//end stDummyTree

#include <arboretum/stDummyTree-inl.h>

#endif //__STDUMMYTREE_H
