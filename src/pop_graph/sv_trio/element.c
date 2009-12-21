/*
 Elemenx.c -- implements the nodes of the dBruijn graph
 */

#include <element.h>
#include <stdlib.h>
#include <global.h>
#include <stdio.h>
#include <string.h>


//currently noone calls this in normal use
// In normal use, the priority queue allocates space to put the eloement directly within,
// and calls element_initialise
Element* new_element()
{


  Element* e = malloc(sizeof(Element));

  if (e==NULL)
    {
      printf("Unable to allocate a new element");
      exit(1);
    }
  
  binary_kmer_initialise_to_zero(&(e->kmer));

  int i;
  for (i=0; i< NUMBER_OF_INDIVIDUALS_PER_POPULATION; i++)
    {
      e->individual_edges[i]=0;
      e->coverage[i]=0;
    }


  e->status=none;
  
  return e;
}


void free_element(Element** element)
{
  free(*element);
  *element=NULL;
}

void element_assign(Element* e1, Element* e2)
{

  binary_kmer_assignment_operator( (*e1).kmer, (*e2).kmer);
  int i;
  for (i=0; i< NUMBER_OF_INDIVIDUALS_PER_POPULATION; i++)
    {
      e1->individual_edges[i] = e2->individual_edges[i];
      e1->coverage[i]         = e2->coverage[i];
    }
  e1->status = e2->status;
}


//gets you a pointer to the edge you are referring to
Edges* get_edge(Element e, EdgeArrayType type,int index)
{
  if (type == individual_edge_array)
    {
      if (index>=NUMBER_OF_INDIVIDUALS_PER_POPULATION)
	{
	  printf("Called get_edge with index %d which is >= NUMBER_OF_INDIVIDUALS_PER_POPULATION which is %d. Exist\n", index, NUMBER_OF_INDIVIDUALS_PER_POPULATION);
	  exit(1);
	}
      return &e.individual_edges[index];//compiler thinks this is returning a local variable, but assuming e is in the hash table, it has a lifetime beyond that of this function.
    }
 else 
    {
      printf("Coding error. Only expecting enum of edge array types to contain one type - individual_edge_array, but we are getting type %d", type);
      exit(1);
    }

  exit(1);
}


//return a copy of the edge you are referring to
Edges get_edge_copy(const Element e, EdgeArrayType type,int index)
{

  if (type == individual_edge_array)
    {
      if (index>=NUMBER_OF_INDIVIDUALS_PER_POPULATION)
	{
	  exit(1);
	}
      return e.individual_edges[index];
    }
  else 
    {
      printf("Coding error. Only expecting enum of edge array types to contain one type - individual_edge_array, but we are getting type %d", type);
      exit(1);
    }

  exit(1);
}



Edges get_union_of_edges(Element e)
{

  int i;
  Edges edges=0;

  for (i=0; i< NUMBER_OF_INDIVIDUALS_PER_POPULATION; i++)
    {
      edges |= e.individual_edges[i];
    }

  return edges;
}







//adds edges from edge_char to the appropriate person/population edgeset, without removing existing edges
void add_edges(Element* e, EdgeArrayType type, int index, Edges edge_char)
{
  if (type == individual_edge_array)
    {
      if (index>=NUMBER_OF_INDIVIDUALS_PER_POPULATION)
	{
	  printf("in element's add_edges function. index is %d, and should be at most %d", index, NUMBER_OF_INDIVIDUALS_PER_POPULATION);
	  exit(1);
	}
      e->individual_edges[index] |= edge_char;
    }

  else
    {
      printf("Coding error. Only expecting enum of edge array types to contain one type - individual_edge_array, but we are getting type %d", type);
      exit(1);
    }
  
}


void set_edges(Element* e, EdgeArrayType type, int index, Edges edge_char)
{
  if (type == individual_edge_array)
    {
      if (index>=NUMBER_OF_INDIVIDUALS_PER_POPULATION)
	{
	  printf("in element's set_edges function. index is %d,and should be at most %d", index, NUMBER_OF_INDIVIDUALS_PER_POPULATION);
	  exit(1);
	}
      e->individual_edges[index] = edge_char;
    }

  else
    {
      printf("Coding error. Only expecting enum of edge array types to contain one type - individual_edge_array, but we are getting type %d", type);
      exit(1);
    }
  
}


void db_node_reset_all_edges_for_all_people_and_pops_to_zero(Element* e)
{
  int i;

    for (i=0; i<NUMBER_OF_INDIVIDUALS_PER_POPULATION; i++)
    {
      e->individual_edges[i]=0;
    }

}

void reset_one_edge(Element* e, Orientation orientation, Nucleotide nucleotide, EdgeArrayType type, int index)
{
  if (type == individual_edge_array)
    {
      if (index>=NUMBER_OF_INDIVIDUALS_PER_POPULATION)
	{
	  printf("in element's reset_one_edge function. index is %d,and should be at most %d", index, NUMBER_OF_INDIVIDUALS_PER_POPULATION);
	  exit(1);
	}

      char edge = 1 << nucleotide;      
      if (orientation == reverse){
	edge <<= 4;
      }
      //toggle 1->0 0->1
      edge ^= (unsigned char) 0xFF; //xor with all 1's, ie 00010000 -> 11101111
      
      e->individual_edges[index] &= edge; //reset one edge
      

    }
  else
    {
      printf("Coding error. Only expecting enum of edge array types to contain one type - individual_edge_array, but we are getting type %d", type);
      exit(1);
    }
  
}


int element_get_number_of_people_or_pops_containing_this_element(Element* e, EdgeArrayType type, int index)
{
  int i;
  int count=0;
  if (type == individual_edge_array)
    {
      for (i=0; i< NUMBER_OF_INDIVIDUALS_PER_POPULATION; i++)
	{
	  if ( (e->individual_edges)[i] != 0)
	    {
	      count++;
	    }
	}
    }
  else
    {
      printf("Coding error. Only expecting enum of edge array types to contain one type - individual_edge_array, but we are getting type %d", type);
      exit(1);
    }

  return count;
}

boolean element_smaller(Element  e1, Element e2){
 
  return get_union_of_edges(e1)  <  get_union_of_edges(e2);
  
}



//WARNING - this gives you a pointer to a the binary kmer in the node. You could modify contents of the hash table
BinaryKmer* element_get_kmer(Element * e){
  return &(e->kmer);
}

boolean element_is_key(Key key, Element e, short kmer_size){
  //  return key == e.kmer;
  return binary_kmer_comparison_operator(*key, e.kmer);
}

Key element_get_key(BinaryKmer* kmer, short kmer_size, Key preallocated_key){
  
  BinaryKmer local_rev_kmer;
  binary_kmer_initialise_to_zero(&local_rev_kmer);

  binary_kmer_reverse_complement(kmer,kmer_size, &local_rev_kmer);
  
  if (binary_kmer_less_than(local_rev_kmer,*kmer, kmer_size))
    {
      binary_kmer_assignment_operator(*((BinaryKmer*)preallocated_key),local_rev_kmer);
    }
  else
    {
      binary_kmer_assignment_operator(*((BinaryKmer*)preallocated_key),*kmer);
    }

  return preallocated_key;

}


void element_initialise(Element * e, Key kmer, short kmer_size){

  if (e==NULL)
    {
      printf("Called elemtn_initialise on NULL ptr");
      exit(1);
    }

  BinaryKmer tmp_kmer;
  binary_kmer_initialise_to_zero(&tmp_kmer);
  binary_kmer_assignment_operator( e->kmer, *(element_get_key(kmer, kmer_size, &tmp_kmer)));

  //hash table has calloc-ed all elements, so elements fromm the hash table are already initialised to zero.
  //however this function is used to reset to 0 Elements that are reused,
  // - see below in the read_binary functions. Also in tests.
  int i;
  for (i=0; i<NUMBER_OF_INDIVIDUALS_PER_POPULATION; i++)
    {
      e->individual_edges[i]=0;
      e->coverage[i]=0;
    }

  db_node_set_status(e, none);

}


void db_node_increment_coverage(dBNode* e, EdgeArrayType type, int index)
{
  if (e==NULL)
    {
      printf("Called increment covg on NULL pointer");
      exit(1);
    }
  e->coverage[index]=e->coverage[index]+1;
}

void db_node_update_coverage(dBNode* e, EdgeArrayType type, int index, short update)
{

  e->coverage[index] += update;

}

//coverage stored as short, but we want to deal with it as an int. So access it via this getter
int db_node_get_coverage(const dBNode* const e, EdgeArrayType type, int index)
{

  if (e==NULL)
    {
      return 0;
    }
  else
    {
      short c = e->coverage[index];
      return (int) c;
    }
}

short db_node_get_coverage_as_short(dBNode* e, EdgeArrayType type, int index)
{
  if (e==NULL)
    { 
      return 0;
    }
  else
    {
      return e->coverage[index];
    }
}


int db_node_get_coverage_in_subgraph_defined_by_func_of_colours(const dBNode* const e, int (*get_covg)(const dBNode*) )
{

  if (e==NULL)
    {
      return 0;
    }
  else
    {
      return get_covg(e);
    }
}



Orientation opposite_orientation(Orientation o){
  return o ^ 1;
  
}

Orientation db_node_get_orientation(BinaryKmer* k, dBNode * e, short kmer_size){

  if (binary_kmer_comparison_operator(e->kmer,*k)==true)
    {
      return forward;
    }
  
  BinaryKmer tmp_kmer;

  if (binary_kmer_comparison_operator(e->kmer, *(binary_kmer_reverse_complement(k,kmer_size, &tmp_kmer)))==true)
    {
      return reverse;
    }
  
  printf("programming error - you have called  db_node_get_orientation with a kmer that is neither equal to the kmer in this node, nor its rev comp\n");
  char tmpseq1[kmer_size];
  char tmpseq2[kmer_size];
  printf("Arg 1 Kmer is %s and Arg 2 node kmer is %s\n", binary_kmer_to_seq(k, kmer_size, tmpseq1), binary_kmer_to_seq(&(e->kmer), kmer_size, tmpseq2));
  exit(1);
  
}





//After specifying which individual or population you are talking about, this function
//adds one edge ("arrow") to the appropriate edge in the appropriate array in the element -- basically sets a bit in the correct edges char
void db_node_add_labeled_edge(dBNode * e, Orientation o, Nucleotide base, EdgeArrayType edge_type, int edge_index){

  //set edge 
  char edge = 1 << base; // A (0) -> 0001, C (1) -> 0010, G (2) -> 0100, T (3) -> 1000
  
  if (o == reverse){
    edge <<= 4; //move to next nibble 
  }

  //update node
  add_edges(e, edge_type, edge_index, edge);
  
}


//adding an edge between two nodes implies adding two labeled edges (one in each direction)
//be aware that in the case of self-loops in palindromes the two labeled edges collapse in one

boolean db_node_add_edge(dBNode * src_e, dBNode * tgt_e, Orientation src_o, Orientation tgt_o, short kmer_size, EdgeArrayType edge_type, int edge_index){

  BinaryKmer src_k, tgt_k, tmp_kmer; 
  char seq1[kmer_size];
  char seq2[kmer_size];

  binary_kmer_assignment_operator(src_k, src_e->kmer);
  binary_kmer_assignment_operator(tgt_k, tgt_e->kmer);

  char tmp_seq[kmer_size];
 
  if (src_o == reverse){
    binary_kmer_assignment_operator(src_k, *(binary_kmer_reverse_complement(&src_k,kmer_size, &tmp_kmer)));
  }
    
  if (tgt_o == reverse){
    binary_kmer_assignment_operator(tgt_k, *(binary_kmer_reverse_complement(&tgt_k,kmer_size, &tmp_kmer)));
  }
    
  
  if (DEBUG){
    printf("add edge %s -%c-> %s to edge type %d, and edge index %d\n",
	   binary_kmer_to_seq(&src_k,kmer_size, seq1),
	   binary_nucleotide_to_char(binary_kmer_get_last_nucleotide(&tgt_k)),
	   binary_kmer_to_seq(&tgt_k,kmer_size, seq2), edge_type, edge_index);
  }

  db_node_add_labeled_edge(src_e,src_o,binary_kmer_get_last_nucleotide(&tgt_k), edge_type, edge_index);

  if (DEBUG){

    printf("add edge %s -%c-> %s to edge type %d, and edge index %d\n",
	   binary_kmer_to_seq(&tgt_k,kmer_size,seq1),
	   binary_nucleotide_to_char(binary_kmer_get_last_nucleotide(binary_kmer_reverse_complement(&src_k,kmer_size, &tmp_kmer))),
	   binary_kmer_to_seq(&src_k,kmer_size, seq2),  edge_type, edge_index);
  }

  db_node_add_labeled_edge(tgt_e,opposite_orientation(tgt_o),binary_kmer_get_last_nucleotide(binary_kmer_reverse_complement(&src_k,kmer_size, &tmp_kmer)), edge_type, edge_index );

  return true;
}



boolean db_node_edge_exist(dBNode * element,Nucleotide base,Orientation orientation, EdgeArrayType edge_type, int edge_index){

  //get the edge char for this specific person or pop:
  char edge = get_edge_copy(*element, edge_type, edge_index);


  edge >>= base;
  if (orientation == reverse){
    edge >>= 4;
  }
  
  edge &= 1;
  
  if (edge == 1){
    return true;
  }
  else{
    return false;
  }
}


//final argument f is a function that returns an Edge that is a function of the different colured edges in a node.
// eg we might be interested in the union of all the coloured edges in the graph, or just the colour/edge for the first person,
//    or the union of all edges except that corresponding to the reference.
boolean db_node_edge_exist_within_specified_function_of_coloured_edges(dBNode * element,Nucleotide base,Orientation orientation, Edges (*f)( const Element* ))
{

  char edge = f(element);


  edge >>= base;
  if (orientation == reverse){
    edge >>= 4;
  }
  
  edge &= 1;
  
  if (edge == 1){
    return true;
  }
  else{
    return false;
  }
  
}






void db_node_reset_edges(dBNode * node,EdgeArrayType edge_type, int edge_index){
  set_edges(node, edge_type, edge_index, 0);
}

void db_node_reset_edge(dBNode * node, Orientation orientation, Nucleotide nucleotide, EdgeArrayType edge_type, int edge_index){
  reset_one_edge(node, orientation, nucleotide, edge_type, edge_index);
}

//pass in a function which will apply reset_one_edge to whichever of the edges it cares about
void db_node_reset_specified_edges(dBNode * node, Orientation orientation, Nucleotide nucleotide,  
				   void (*f)(dBNode*, Orientation, Nucleotide )){
  f(node, orientation, nucleotide);
}




boolean db_node_edges_reset(dBNode * node, EdgeArrayType edge_type, int edge_index){
  return get_edge_copy(*node,edge_type,edge_index) == 0;
}


boolean db_node_has_precisely_one_edge(dBNode * node, Orientation orientation, Nucleotide * nucleotide, EdgeArrayType edge_type, int edge_index){
  
  Nucleotide n;
  Edges edges = get_edge_copy(*node,edge_type,edge_index);
  short edges_count = 0;

  if (orientation == reverse){
    edges >>= 4;
  }
 
  
  for(n=0;n<4;n++){
    
    if ((edges & 1) == 1){
      *nucleotide = n;
      edges_count++;
    }
    
    edges >>= 1;    
  }
  
  return (edges_count == 1);
  
}



boolean db_node_has_precisely_one_edge_in_subgraph_defined_by_func_of_colours(dBNode * node, Orientation orientation, Nucleotide * nucleotide, 
									      Edges (*get_colour)(const dBNode*) )
{
  
  Nucleotide n;
  Edges edges = get_colour(node);
  short edges_count = 0;

  if (orientation == reverse){
    edges >>= 4;
  }
 
  
  for(n=0;n<4;n++){
    
    if ((edges & 1) == 1){
      *nucleotide = n;
      edges_count++;
    }
    
    edges >>= 1;    
  }
  
  return (edges_count == 1);
  
}


boolean db_node_has_precisely_one_edge_in_union_graph_over_all_people(dBNode * node, Orientation orientation, Nucleotide * nucleotide){
  
  Nucleotide n;
  Edges edges = get_union_of_edges(*node);
  short edges_count = 0;

  if (orientation == reverse){
    edges >>= 4;
  }
 
  
  for(n=0;n<4;n++){
    
    if ((edges & 1) == 1){
      *nucleotide = n;
      edges_count++;
    }
    
    edges >>= 1;    
  }
  
  return (edges_count == 1);
  
}

//a conflict - bifurcation
boolean db_node_has_precisely_two_edges(dBNode * node, Orientation orientation, Nucleotide * nucleotide1, Nucleotide * nucleotide2, EdgeArrayType type, int index){
  
  Nucleotide n;

  Edges edges = get_edge_copy(*node,type,index);

  short edges_count = 0;

  if (orientation == reverse){
    edges >>= 4;
  }
  
  for(n=0;n<4;n++){
    
    if ((edges & 1) == 1){
      if (edges_count == 0){
	*nucleotide1 = n;
      }
      
      if (edges_count == 1){
	*nucleotide2 = n;
      }
      edges_count++;
    }
    
    edges >>= 1;    
  }
  
  return (edges_count == 2);
  
}



boolean db_node_is_blunt_end(dBNode * node, Orientation orientation, EdgeArrayType edge_type, int edge_index){
  
  Edges edges = get_edge_copy(*node, edge_type, edge_index);


  if (orientation == reverse){
    edges >>= 4;
  }
  
  edges &= 15; // AND with 00001111 so that we only look at the 4 least significant bits
  
  return edges == 0;
}

boolean db_node_check_status(dBNode * node, NodeStatus status){
  return node->status == status;
}

boolean db_node_check_status_not_pruned(dBNode * node){
  if ( db_node_check_status(node, none) || db_node_check_status(node,visited))
    {
      return true;
    }
  return false;
}


boolean db_node_check_status_not_pruned_or_visited(dBNode * node)
{
  if ( db_node_check_status(node,visited) || db_node_check_status(node,visited_and_exists_in_reference) ||  db_node_check_status(node, pruned)	 ) 
    {
      return false;
    }
  else
    {
      return true;
    }
}


void db_node_set_status(dBNode * node,NodeStatus status){
  node->status = status;
}
void db_node_set_status_to_none(dBNode * node){
  node->status = none;
}




boolean db_node_is_this_node_in_this_person_or_populations_graph(dBNode* node, EdgeArrayType type, int index)
{

  if (node ==NULL)
    {
      return false;
    }

  Edges edge_for_this_person_or_pop = get_edge_copy(*node, type, index);

  if (edge_for_this_person_or_pop == 0)
    {
      return false;
    }
  else
    {
      return true;
    }
 
}

boolean db_node_is_this_node_in_subgraph_defined_by_func_of_colours(dBNode* node, Edges (*get_colour)(const dBNode*) )
{

  if (node ==NULL)
    {
      return false;
    }

  //get_colour will return an edge which is some function of all the edges/colours at that node. eg, will AND all the edges, to consider the union of all colours, etc
  Edges edge = get_colour(node);

  if (edge == 0)
    {
      return false;
    }
  else
    {
      return true;
    }
 
}


void db_node_print_binary(FILE * fp, dBNode * node)
{

  BinaryKmer kmer;
  binary_kmer_assignment_operator(kmer, *element_get_kmer(node) );
  short covg[NUMBER_OF_INDIVIDUALS_PER_POPULATION];
  Edges individual_edges[NUMBER_OF_INDIVIDUALS_PER_POPULATION]; 

  int i;
  for (i=0; i< NUMBER_OF_INDIVIDUALS_PER_POPULATION; i++)                                                     
    {                                                                                                         
      covg[i] = db_node_get_coverage_as_short(node, individual_edge_array, i);
      individual_edges[i]= get_edge_copy(*node, individual_edge_array, i);
    }      
				  
  fwrite(&kmer, NUMBER_OF_BITFIELDS_IN_BINARY_KMER*sizeof(bitfield_of_64bits), 1, fp);
  fwrite(covg, sizeof(short), NUMBER_OF_INDIVIDUALS_PER_POPULATION, fp); 
  fwrite(individual_edges, sizeof(Edges), NUMBER_OF_INDIVIDUALS_PER_POPULATION, fp);

  
}


boolean db_node_read_sv_trio_binary(FILE * fp, short kmer_size, dBNode * node){

  BinaryKmer kmer;
  binary_kmer_assignment_operator(kmer, *(element_get_kmer(node)) );
  short covg[NUMBER_OF_INDIVIDUALS_PER_POPULATION];
  Edges individual_edges[NUMBER_OF_INDIVIDUALS_PER_POPULATION]; 

  int read;

  read = fread(&kmer,sizeof(bitfield_of_64bits)*NUMBER_OF_BITFIELDS_IN_BINARY_KMER,1,fp);  

  if (read>0){


    read = fread(covg, sizeof(short), NUMBER_OF_INDIVIDUALS_PER_POPULATION, fp);    
    if (read==0){
      puts("error with input file - failed to read covg in db_node_read_sv_trio_binary\n");
      exit(1);
    }

    read = fread(individual_edges, sizeof(Edges), NUMBER_OF_INDIVIDUALS_PER_POPULATION, fp);
    if (read==0){
      puts("error with input file - failed to read Edges in db_node_read_sv_trio_binary\n");
      exit(1);
    }



  }
  else{
    return false;
  }

  element_initialise(node,&kmer,kmer_size);

  int i;
  for (i=0; i< NUMBER_OF_INDIVIDUALS_PER_POPULATION; i++)
    {
      node->coverage[i]         = covg[i];
      node->individual_edges[i] = individual_edges[i];
    }

  return true;
}

//read a binary for an individual person, as dumped by the target "graph"
// the edge array type and index tell you which person you should load this data into
boolean db_node_read_graph_binary(FILE * fp, short kmer_size, dBNode * node, EdgeArrayType type, int index)
{

  if ( (index<0) || (index>=NUMBER_OF_INDIVIDUALS_PER_POPULATION))
    {
      printf("Invalid index for which person to load binary into: %d. Exiting.", index);
      exit(1);
    }

  BinaryKmer kmer;
  Edges edges;
  short coverage;
  int read;
  
  read = fread(&kmer,sizeof(bitfield_of_64bits)*NUMBER_OF_BITFIELDS_IN_BINARY_KMER,1,fp);

  if (read>0){
    read = fread(&coverage,sizeof(short),1,fp);    
    if (read==0){
      puts("error with input file\n");
      exit(1);
    }
    read = fread(&edges,sizeof(Edges),1,fp);
    if (read==0){
      puts("error with input file\n");
      exit(1);
    }   
  }
  else{
    return false;
  }

  element_initialise(node,&kmer,kmer_size);
  node->individual_edges[index]    = edges;
  node->coverage[index] = coverage;
  return true;
}


void db_node_action_set_status_none(dBNode * node){
  db_node_set_status(node,none);
}

void db_node_action_set_status_pruned(dBNode * node){
  db_node_set_status(node,pruned);
}


void db_node_action_set_status_visited(dBNode * node){
  db_node_set_status(node,visited);
}

void db_node_action_set_status_visited_or_visited_and_exists_in_reference(dBNode * node){

  if (db_node_check_status(node, exists_in_reference))
    {
      db_node_set_status(node,visited_and_exists_in_reference);      
    }
  //WARNING. Need special case for pruned?
  else if (!db_node_check_status(node, unassigned)) 
    {
      db_node_set_status(node,visited);
    }

}


void db_node_action_unset_status_visited_or_visited_and_exists_in_reference(dBNode * node){
  if (db_node_check_status_visited_and_exists_in_reference(node))
  {
    db_node_set_status(node,exists_in_reference);
  }
  else if (db_node_check_status(node, visited))
  {
    db_node_set_status(node, none);
  }
      
}



void db_node_action_do_nothing(dBNode * node){
  
}


boolean db_node_check_status_none(dBNode * node){
  return db_node_check_status(node,none);
}

boolean db_node_check_status_visited(dBNode * node){
  return db_node_check_status(node,visited);
}

boolean db_node_check_status_exists_in_reference(dBNode * node){
  return db_node_check_status(node,exists_in_reference);
}

boolean db_node_check_status_visited_and_exists_in_reference(dBNode * node){
  return db_node_check_status(node,visited_and_exists_in_reference);
}

boolean db_node_check_status_is_not_exists_in_reference(dBNode * node){

  if (db_node_check_status(node,exists_in_reference)==true )
    {
      return false;
    }
  else
    {
      return true;
    }
}

boolean db_node_check_status_is_not_visited(dBNode* node)
{
   if (db_node_check_status(node,visited)  )
    {
      return false;
    }
  else
    {
      return true;
    }
  
}

boolean db_node_check_status_is_not_visited_or_visited_and_exists_in_reference(dBNode * node){
  if (db_node_check_status(node,visited_and_exists_in_reference) || db_node_check_status(node,visited)  )
    {
      return false;
    }
  else
    {
      return true;
    }
}


boolean db_node_condition_always_true(dBNode* node)
{
  return true;
}



// wrapper to allow comparison with read_start_forward OR read_start_forward_and_reverse
// ie. if you want to check if this node is the start of a read (forwards), then call this, with second argument forward
// and it will handle the case when it is a read start in both directions
boolean db_node_check_read_start(dBNode* node, Orientation ori)
{
  if ( (ori==forward) && 
       (db_node_check_status(node, read_start_forward) || db_node_check_status(node, read_start_forward_and_reverse) ) ) 
    {
      return true;
    }
  else if ( (ori==reverse) && 
       (db_node_check_status(node, read_start_reverse) || db_node_check_status(node, read_start_forward_and_reverse) ) ) 
    {
      return true;
    }
  else
    {
      return false;
    }
}

void db_node_set_read_start_status(dBNode* node, Orientation ori)
{
  if (db_node_check_status(node, visited) )
    {
      printf("Warning - setting status of a visisted node to read_start\n");
    }
  else if (db_node_check_status(node, unassigned) )
    {
      printf("Warning - setting status of an unassigned node to read_start. Exit");
      exit(1);
    }
  else if (db_node_check_status(node, read_start_forward) && (ori==reverse) )
    {
      db_node_set_status(node, read_start_forward_and_reverse);
    }
  else if (db_node_check_status(node, read_start_reverse) && (ori==forward) )
    {
      db_node_set_status(node, read_start_forward_and_reverse);
    }
  else if (ori==forward)
    {
      db_node_set_status(node, read_start_forward);
    }
  else if (ori==reverse)
    {
      db_node_set_status(node, read_start_reverse);
    }
  else
    {
      printf("Programming error in read start setting of node");
      exit(1);
    }
  
}



boolean db_node_check_duplicates(dBNode* node1, Orientation o1, dBNode* node2, Orientation o2)
{
  if (      (o1==forward) && (o2==forward) && db_node_check_read_start(node1, forward) && db_node_check_read_start(node2, forward) )
    {
      return true;
    }
  else if ( (o1==reverse) && (o2==forward) && db_node_check_read_start(node1, reverse) && db_node_check_read_start(node2, forward) )
    {
      return true;
    }
  else if ( (o1==forward) && (o2==reverse) && db_node_check_read_start(node1, forward) && db_node_check_read_start(node2, reverse) )
    {
      return true;
    }
  else if ( (o1==reverse) && (o2==reverse) && db_node_check_read_start(node1, reverse) && db_node_check_read_start(node2, reverse) )
    {
      return true;
    }
  else
    {
      return false;
    }
}

//we have a read that starts at node in direction o1, and we want to know if a previous read started at that node in that direction
boolean db_node_check_single_ended_duplicates(dBNode* node1, Orientation o1)
{
  if (      (o1==forward) && db_node_check_read_start(node1, forward)  )
    {
      return true;
    }
  else if ( (o1==reverse) && db_node_check_read_start(node1, reverse) )
    {
      return true;
    }
  else
    {
      return false;
    }
}
