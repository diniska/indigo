/****************************************************************************
 * Copyright (C) 2009-2011 GGA Software Services LLC
 * 
 * This file is part of Indigo toolkit.
 * 
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 3 as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPL included in the
 * packaging of this file.
 * 
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 ***************************************************************************/

#ifndef _reaction_automapper
#define _reaction_automapper

#include "base_cpp/array.h"
#include "base_cpp/ptr_array.h"
#include "molecule/max_common_submolecule.h"

namespace indigo {

class BaseReaction;

//util class for keeping map generating from aam in reaction
class ReactionMapMatchingData{
public:
   ReactionMapMatchingData(BaseReaction& r);
   //sets maps for atoms in molecules
   void createAtomMatchingData();
   //sets maps for bonds in molecules
   void createBondMatchingData();

   int beginMap(int mol_idx) const { return nextMap(mol_idx, -1); }
   int endMap() const ;
   int nextMap(int mol_idx, int opp_idx) const;


   int beginAtomMap(int mol_idx, int atom_idx) const { return nextAtomMap(mol_idx, -1, atom_idx); }
   int endAtomMap() const; 
   int nextAtomMap(int mol_idx, int opposite_idx, int atom_idx) const;
   bool getAtomMap(int mol_idx, int opposite_idx, int atom_idx, Array<int>* mapping) const;
   
   int beginBondMap(int mol_idx, int bond_idx) const { return nextBondMap(mol_idx, -1, bond_idx); }
   int endBondMap() const; 
   int nextBondMap(int mol_idx, int opposite_idx, int bond_idx) const;
   bool getBondMap(int mol_idx, int opposite_idx, int bond_idx, Array<int>* mapping) const;

private:
   BaseReaction& _reaction;
   
   //array to set correspondence between atoms due to input mapping
   ObjArray< Array<int> > _vertexMatchingArray;
   //array to set correspondence between bonds due to input mapping
   ObjArray< Array<int> > _edgeMatchingArray;

   int _getVertexId(int mol_idx, int vert) const;
   int _getEdgeId(int mol_idx, int edge) const;
};

class ReactionAutomapper {

public:
   enum {
      // disregards any existing maps or bond change marks. 
      AAM_REGEN_DISCARD = 0, 
       // assumes the existing marks are absolutely correct. auto_aam changes only null marks 
      AAM_REGEN_KEEP = 1,
      // assumes the existing marks might be wrong and can be altered. auto_amm consider input mapping and change it if map wrong
      AAM_REGEN_ALTER = 2,
      // special mode for clearing a mapping
      AAM_REGEN_CLEAR = 3,

      MAX_PERMUTATIONS_NUMBER = 5000
   };
   
   ReactionAutomapper(BaseReaction& reaction);
   //main method for searching AAM
   void automap(int mode);

   void correctReactingCenters(bool change_null_map) { _checkAtomMapping(true, false, change_null_map); }

   /*
    * Special flags
    */
   bool ignore_atom_charges;
   bool ignore_atom_valence;
   bool ignore_atom_isotopes;
   bool ignore_atom_radicals;


   DEF_ERROR("Reaction automapper");

private:
   //parameter for dimerization and dissociation
    enum {
      _MIN_VERTEX_SUB = 3
    };
   //sets up input mapping
   void _initMappings(BaseReaction& reaction);
   //searches AAM using mcs and substructure functions
   void _createReactionMap();
   //controls AAM due to reacting centers in reaction; this could change reacting centers or AAM

   void _cleanReactants(BaseReaction& reaction);

   int _handleWithProduct(const Array<int>& reactant_cons, Array<int>& product_mapping_tmp, BaseReaction& reaction, int product, ReactionMapMatchingData& react_map_match) ;
   void _chooseBestMapping(BaseReaction& reaction, Array<int>& product_mapping, int product, int map_complete);
   bool _checkAtomMapping(bool change_rc, bool change_aam, bool change_rc_null);

   //arranges all maps to AAM
   void _setupReactionMap(Array<int> &mol_mapping, ObjArray< Array<int> >& mappings);
   //takes account of possibility for molecule dissociation 
   void _considerDissociation();

   //takes account of possibility for molecule dimerization
   void _considerDimerization();
   
   int _validMapFound(BaseReaction& reaction, int react, int prod, Array<int>& sub_map) const;
   void _removeSmallComponents(BaseMolecule& mol) const ;
   //all permutation 
   static void _permutation(Array<int>& ,ObjArray< Array<int> > &);


   BaseReaction& _initReaction;
   AutoPtr<BaseReaction> _reactionCopy;

   Array<int> _usedVertices;
   int _maxMapUsed;
   int _maxVertUsed;
   int _maxCompleteMap;
   int _mode;
};


class RSubstructureMcs : public SubstructureMcs {
public:
   enum {
       // Conditions
      CONDITION_NONE      = 0x0000,
      CONDITION_ATOM_CHARGES = 0x0001, 
      CONDITION_ATOM_VALENCE = 0x0002, 
      CONDITION_ATOM_RADICAL = 0x0004, 
      CONDITION_ATOM_ISOTOPES = 0x0008, 
      CONDITION_ALL       = 0x000F,
      
      //maximum iteratins in exact mcs method
      MAX_ITERATION_NUMBER = 50000,
      HIGH_PRIORITY_SCORE = 1000

   };
   RSubstructureMcs(BaseReaction& reaction, int subNum, int superNum, const ReactionAutomapper& context);
   RSubstructureMcs(BaseReaction &reaction, BaseMolecule& sub, BaseMolecule& super, const ReactionAutomapper& context);
   virtual ~RSubstructureMcs(){}

   //reaction molecules substructure search 
   bool searchSubstructureReact(BaseMolecule& init_rmol, const Array<int>* in_map, Array<int> *out_map);
   //reaction molecules mcs search
   bool searchMaxCommonSubReact(const Array<int>* in_map, Array<int> *out_map);

   //callback for atom matching
   static bool atomConditionReact (Graph &g1, Graph &g2, const int *core_sub, int i, int j, void* userdata);
   //callback for bonds matching
   static bool bondConditionReact (Graph &g1, Graph &g2, int i, int j, void* userdata);
   //callback for bonds matching strict rules
   static bool bondConditionReactStrict (Graph &g1, Graph &g2, int i, int j, void* userdata);
   // simple exact bond matching
   static bool bondConditionReactSimple (Graph &g1, Graph &g2, int i, int j, void* userdata);

      //callback for mcs sorting solutions
   static int cbMcsSolutionTerm(Array<int>&, Array<int>&, void*);

   int findReactionCenter (BaseMolecule &mol, int bondNum) const ;
   void getReactingCenters(BaseMolecule& mol1, BaseMolecule& mol2, int bond1, int bond2, int& rc_reactant, int& rc_product) const;

   inline int getReactNumber() const { return _subReactNumber; }
   inline int getProductNumber() const { return _superProductNumber;}
   BaseReaction& getReaction() const { return _reaction; }

   void setUpFlags(const ReactionAutomapper& context);

   int flags;

private:
   int _searchSubstructure(EmbeddingEnumerator& emb_enum, const Array<int>* in_map, Array<int> *out_map);
   static bool _matchAtoms(BaseMolecule& query, BaseMolecule& target, int sub_idx, int super_idx, int flags);

   void _selectBestAutomorphism(Array<int>* map_out);

   static int  _cbAutoVertexReact (Graph &graph, int idx1, int idx2, const void *context);
   static bool _cbAutoCheckAutomorphismReact (Graph &graph, const Array<int> &mapping, const void *context);
   int _scoreSolution(Molecule *sub_molecule, Molecule *super_molecule, Array<int>& map);
   
   BaseReaction& _reaction;
   int _subReactNumber;
   int _superProductNumber;
   
   ObjArray< Array<int> > _autoMaps;
};

}

#endif
