#pragma once
/*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
SPoint2u	= uint16_t x,y
CPathArray	= Array class for finded path
CDumbPather = understend)

Use:
	#define DUMBPATHER_TYPES		// !!!!!!!!!!!
	#define DUMBPATHER_DBGOUT		// If you need debug out
	#define DUMBPATHER_DEFAULT_FAST	// Select adjacent cost mode [standart = UNWOKABLE mode]
	#define SPoint2u Point2D		// You can use any struct, if it have [x,y] freandly for [CDumbPather]
	#include " [include_lib_directory] //dumbpather.h"

	############################################################
	use only binar map size if you use USE_DUMB_OPTIMIZATE_MAP!

	CELL = one byte node

			   - 8 7 6 5  4 3 2 1 -
		CELL = [ 0 0 0 0  0 0 0 0 ]

			1 2 3
			4 * 5
			6 7 8

	CELL & 0b00000001 => cost of move to { y-1 , x-1 }
	CELL & 0b00000010 => cost of move to { y-1 , x   }
	CELL & 0b00000100 => cost of move to { y-1 , x+1 }
	CELL & 0b00001000 => cost of move to {   y , x-1 }
	CELL & 0b00010000 => cost of move to {   y , x+1 }
	CELL & 0b00100000 => cost of move to { y+1 , x-1 }
	CELL & 0b01000000 => cost of move to { y+1 , x   }
	CELL & 0b10000000 => cost of move to { y+1 , x+1 }

	#define USE_DUMB_OPTIMIZATE_MAP

	#############################################################

	struct Point2D{
		u_2b x;
		u_2b y;
		* * * //You something interface realisation
	}
	    * * *
		SomeMapClass Map;
		SPoint2u points[2] = {
			{ unit_start_XY_position },
			{ unit_end_XY_position }
		};
		#ifdef USE_DUMB_FLAT_MAP_MODE
		i_1b* MapCellsMatrix  =  Map.getCellsMatix(); // << ACHTUNG акхтунгк
		#else
		i_1b** MapCellsMatrix =  Map.getCellsMatix(); // << ACHTUNG акхтунгк
		endif
		u_2b   MapSize		  =  Map.getSize();

		pthfd::CDumbPather PathManager;
		pthfd::CPathArray UnitFindedPath;

		PathManager.SetByteMap( MapCellsMatrix, MapSize );
		PathManager.FindPath( points, &UnitFindedPath );

		for(int i=0; i<UnitFindedPath.Length())
			Something_Procedure_With_a_Path_Points(UnitFindedPath[i]);

 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 */
#ifndef PATHFINDER_FOR_DUMBS_INCLUDE
#define PATHFINDER_FOR_DUMBS_INCLUDE
#define Active_OUTSTREAM(text) //std::cout<< text << std::endl;

#ifdef _MSC_VER
#ifdef _DEBUG
	#ifdef DUMBPATHER_DBGOUT
		#define dbg_out(s) Active_OUTSTREAM(s)
	#else // !DUMBPATHER_DBGOUT
		#define dbg_out(s)
	#endif // !DUMBPATHER_DBGOUT
#else // !_DEBUG
	#define dbg_out(s)
	#pragma warning( disable : 4786 )	// Debugger truncating names.
	#pragma warning( disable : 4530 )	// Exception handler isn't used
#endif // !_DEBUG
#endif

#define MAXIMAL 0x7FFFFFFFu
#define CACHBLOCK 512

#ifdef DUMBPATHER_TYPES
	#ifdef __khrplatform_h_
		typedef khronos_int8_t i_1b;
		typedef khronos_uint8_t u_1b;
		typedef khronos_int16_t i_2b;
		typedef khronos_uint16_t u_2b;
		typedef khronos_int32_t i_4b;
		typedef khronos_uint32_t u_4b;
		typedef khronos_int64_t i_8b;
		typedef khronos_uint64_t u_8b;
	#else
		#ifdef _STDINT
			typedef int8_t i_1b;
			typedef uint8_t u_1b;
			typedef int16_t i_2b;
			typedef uint16_t u_2b;
			typedef int32_t i_4b;
			typedef uint32_t u_4b;
			typedef int64_t i_8b;
			typedef uint64_t u_8b;
		#else
			typedef signed char i_1b;
			typedef unsigned char u_1b;
			typedef signed short i_2b;
			typedef unsigned short u_2b;
			typedef signed int i_4b;
			typedef unsigned int u_4b;
			typedef signed long long i_8b;
			typedef unsigned long long u_8b;
		#endif
	#endif
	typedef void* PMem;
	typedef const char* c_wrd;
	typedef char* t_wrd;
	typedef const char c_sym;
	typedef char t_sym;
	typedef bool Bool;
	typedef float f_4b;
	typedef double f_8b;

#endif//DUMBPATHER_TYPES

#ifdef USE_DUMB_OPTIMIZATE_MAP
#include <functional>
#include <array>
#define DUMB_OPTIMIZE(declarate) declarate
#define DUMB_UNOPTIMIZE(declarate)
#else //else - USE_DUMB_OPTIMIZATE_MAP
#define DUMB_OPTIMIZE(declarate)
#define DUMB_UNOPTIMIZE(declarate) declarate
//Select adjacent cost mode :
#ifdef DUMBPATHER_DEFAULT_FAST // FAST BLOK IS DEFAULT
//=============================[    FAST BLOK IS DEFAULT   ]=============================
#define SET_LINE_NEGIBORS(typ) if (stepX < map_size_)	{								\
		if ((typ != TerrainType::BLOKABLE))												\
			if (typ == TerrainType::SLOWABLE) neighbors->PushBack({ SLOW_LINE, indx });	\
			else neighbors->PushBack({ FAST_LINE, indx });								}
#define SET_DIAG_NEGIBORS(typ) if (stepX < map_size_)	{								\
		if ((typ != TerrainType::BLOKABLE))												\
			if (typ == TerrainType::SLOWABLE) neighbors->PushBack({ SLOW_DIAG, indx });	\
			else neighbors->PushBack({ FAST_DIAG, indx });								}
#else //else - DUMBPATHER_DEFAULT_FAST
#ifdef DUMBPATHER_DEFAULT_SLOW
//=============================[    SLOW BLOK IS DEFAULT   ]=============================
#define SET_LINE_NEGIBORS(typ)	if (stepX < map_size_)	{								\
		if ((typ != TerrainType::BLOKABLE))												\
			if (typ == TerrainType::WALKABLE) neighbors->PushBack({ FAST_LINE, indx });	\
			else neighbors->PushBack({ SLOW_LINE, indx });								}
#define SET_DIAG_NEGIBORS(typ)	if (stepX < map_size_)	{								\
		if ((typ != TerrainType::BLOKABLE))												\
			if (typ == TerrainType::WALKABLE) neighbors->PushBack({ FAST_DIAG, indx });	\
			else neighbors->PushBack({ SLOW_DIAG, indx });								}
#endif //end - DUMBPATHER_DEFAULT_SLOW
//=============================[ UNWOKABLE BLOK IS DEFAULT ]=============================
#define SET_LINE_NEGIBORS(typ)	if (stepX < map_size_)	{								\
	if (typ == TerrainType::WALKABLE) neighbors->PushBack({ FAST_LINE, indx });			\
	else if (typ == TerrainType::SLOWABLE) neighbors->PushBack({ SLOW_LINE, indx });	}
#define SET_DIAG_NEGIBORS(typ)	if (stepX < map_size_)	{								\
	if (typ == TerrainType::WALKABLE) neighbors->PushBack({ FAST_DIAG, indx });			\
	else if (typ == TerrainType::SLOWABLE) neighbors->PushBack({ SLOW_DIAG, indx });	}
#endif //end - DUMBPATHER_DEFAULT_FAST
#endif //end - USE_DUMB_OPTIMIZATE_MAP

//Select SPoint2u - you can use any struct, if it have [x,y] freandly for [CDumbPather]
#ifndef SPoint2u
#define SPoint2u DumbPathrer_2bytes_2D_point
	struct DumbPathrer_2bytes_2D_point {
		u_2b x;
		u_2b y;
	};
#else
	struct SPoint2u;
#endif // !SPoint2u

namespace pthfd {
	//======================================================================
	enum TerrainType : i_1b {
		WALKABLE = '.',
		BLOKABLE = 'X',
		SLOWABLE = 'Y'
	};
	//======================================================================
	class CPathArray final {
	private:
		SPoint2u* array_;
		u_4b length_;
	public:
		~CPathArray() { 
			dbg_out("CALL DESTRUCTOR [ ~CPathArray ]");
			if (length_) delete[] array_; 
		}
		CPathArray() : array_(nullptr), length_(0) {
			dbg_out("CALL CONSTRUCTOR [ CPathArray ]");
		}
		CPathArray(const CPathArray& other) {
			dbg_out("CALL COPY [ CPathArray ]");
			this->length_ = other.length_;
			this->Relloc(this->length_);
			memcpy(this->array_, other.array_,other.length_ * sizeof(SPoint2u));
		}
		CPathArray(CPathArray&& other) noexcept {
			dbg_out("CALL FORWARD [ CPathArray ]");
			this->array_ = other.array_;
			other.array_ = nullptr;
			this->length_ = other.length_;
			other.length_ = 0;
		}
		CPathArray& operator=(const CPathArray& other) {
			if(&other == this) return *this;
			dbg_out("CALL [ = ] COPY [ CPathArray ]");
			this->length_ = other.length_;
			this->Relloc(this->length_);
			memcpy(other.array_, this->array_, other.length_ * sizeof(SPoint2u));
			return *this;
		}
		CPathArray& operator=(CPathArray&& other) noexcept {
			if (&other == this) return *this;
			dbg_out("CALL [ = ] FORWARD [ CPathArray ]");
			if (length_) delete[] array_;
			this->array_ = other.array_;
			other.array_ = nullptr;
			this->length_ = other.length_;
			other.length_ = 0;
			return *this;
		}
		__forceinline SPoint2u& operator[](u_4b i) { return array_[i]; }
		__forceinline u_4b Length() const { return length_; }
		__forceinline void Relloc(u_4b array_length) {
			if (array_length == length_) return;
			if (array_) delete[] array_;
			if (!array_length) {
				array_ = nullptr;
				length_ = 0;
				return;
			}
			array_ = new SPoint2u[array_length];
			length_ = array_length;
		}
	};
	class CDumbPather final {
	private:
		//======================================================================
		enum : i_4b {
			NO_SOLUTION = -1,
			AT_THE_END  = 1,
			IS_SOLVED   = 2
		};
		enum : u_4b {
			WALL_COST = 16,
			FAST_LINE = 128,
			SLOW_LINE = 256,
			FAST_DIAG = 181,
			SLOW_DIAG = 362
		};
		//======================================================================
		class CPathNode;
		struct SNodeCost final {
			CPathNode* node;
			u_4b cost;
		};
		struct SStateCost final {
			u_4b cost;	//< The cost to the state. Use MAXIMAL for infinite cost.
			u_4b state;	//< The state as a u_4b
		};
		//======================================================================
		template <typename T> 
		class CVector final {
		private:
			u_4b m_allocated; u_4b m_size; T* m_buf;
			__forceinline void capacity(u_4b cap) {
				if(m_allocated < cap) {
					m_allocated = (cap >> 1) + cap + 16;
					do m_buf = reinterpret_cast<T*>( realloc(reinterpret_cast<void*>(m_buf), m_allocated * sizeof(T))  ); 
					while(!m_buf);
				}
			}
		public:
			__forceinline ~CVector() {
				dbg_out("CALL DESTRUCTOR [ ~CVector ]");
				free(m_buf);
			}
			__forceinline CVector() : m_size(0) {
				dbg_out("CALL CONSTRUCTOR [ CVector ]");
				capacity(0);
			}
			__forceinline void Clear() { m_size = 0; }	// see warning above
			__forceinline void Relloc(u_4b s) { capacity(s); m_size = s; }
			__forceinline void PushBack(const T& t) noexcept { capacity(m_size + 1); m_buf[m_size++] = t; }
			__forceinline u_4b Size() const { return m_size; }
			__forceinline T& operator[](u_4b i) { return m_buf[i]; }
		};
		class CPathNode final {
		public:
			CPathNode* child[2];		// Binary search in the hash table. [left, right]
			CPathNode* next, * prev;	// used by open queue
			CPathNode* parent;			// the parent is used to reconstruct the path
			u_4b heapIndex;				// unique id for this path, so the solver can distinguish
			u_4b frame_;				// unique id for this path, so the solver can distinguish
			u_4b state;					// the client state
			u_4b costStart;				// exact
			u_4b estToGoal;				// estimated
			u_4b totalCost;				// could be a function, but save some math.
			i_4b numAdjacent;			// -1  is unknown & needs to be queried
			i_4b cacheIndex;			// position in cache
			Bool CloseFlag;
			Bool OpenFlag;

			__forceinline void Init(u_4b _frame, u_4b _state, u_4b _costFromStart, u_4b _estToGoal, CPathNode* _parent) {
				state = _state;
				costStart = _costFromStart;
				estToGoal = _estToGoal;
				CalcTotalCost();
				parent = _parent;
				frame_ = _frame;
				OpenFlag = 0;
				CloseFlag = 0;
			}
			__forceinline void Clear() {
				memset(this, 0, sizeof(CPathNode));
				numAdjacent = -1;
				cacheIndex = -1;
				heapIndex = MAXIMAL;
			}

			__forceinline void InitSentinel() {
				Clear();
				Init(0, 0, MAXIMAL, MAXIMAL, 0);
				prev = next = this;
			}
			__forceinline void Unlink() {
				next->prev = prev;
				prev->next = next;
				next = prev = 0;
			}
			__forceinline void AddBefore(CPathNode* addThis) {
				addThis->next = this;
				addThis->prev = prev;
				prev->next = addThis;
				prev = addThis;
			}
			__forceinline void CalcTotalCost() {
				totalCost = (costStart < MAXIMAL && estToGoal < MAXIMAL) ? costStart + estToGoal : MAXIMAL;
			}
		};
		class CPathNodePool final {
		private:
			struct Block {
				Block* nextBlock;
				CPathNode pathNode[1];
			};
			CPathNode	freeMemSentinel;
			u_4b		nAllocated;				// number of pathnodes allocated (from Alloc())
			u_4b		nAvailable;				// number available for allocation
			u_4b		hashShift;
			CPathNode** hashTable;
			Block*		firstBlock;
			Block*		blocks;
			SNodeCost*	cache;
			i_4b		cacheCap;
			i_4b		cacheSize;

			__forceinline u_4b HashSize() { return 1 << hashShift; }
			__forceinline u_4b HashMask() { return ((1 << hashShift) - 1); }
			__forceinline void AddCPathNode(u_4b key, CPathNode* root) {
				if (hashTable[key]) {
					CPathNode* p = hashTable[key];
					while (true) {
						i_4b dir = (root->state < p->state) ? 0 : 1;
						if (p->child[dir]) { p = p->child[dir]; }
						else { p->child[dir] = root; break; }
					}
				}
				else { hashTable[key] = root; }
			}
			__forceinline Block* NewBlock() {
				Block* block;
				do block = reinterpret_cast<Block*>(malloc(sizeof(Block) + sizeof(CPathNode) * (CACHBLOCK - 1))); 
				while (!block);
				block->nextBlock = 0;
				nAvailable += CACHBLOCK;
				for (u_4b i = 0; i < CACHBLOCK; ++i)
					freeMemSentinel.AddBefore(&block->pathNode[i]);
				return block;
			}
			__forceinline CPathNode* Alloc() {
				if (freeMemSentinel.next == &freeMemSentinel) {
					Block* b = NewBlock();
					b->nextBlock = blocks;
					blocks = b;
				}
				CPathNode* pathNode = freeMemSentinel.next;
				pathNode->Unlink();
				++nAllocated;
				--nAvailable;
				return pathNode;
			}
		public:
			__forceinline CPathNodePool(u_4b _typicalAdjacent) : firstBlock(0), blocks(0), nAllocated(0), nAvailable(0) {
				dbg_out("CALL CONSTRUCTOR [ CPathNodePool ]");
				freeMemSentinel.InitSentinel();
				cacheCap = CACHBLOCK * _typicalAdjacent;
				cacheSize = 0;
				cache = reinterpret_cast<SNodeCost*>(malloc(cacheCap * sizeof(SNodeCost)));
				hashShift = 3;	// 8 (only useful for stress testing) 
				while (HashSize() < CACHBLOCK) ++hashShift;
				hashTable = reinterpret_cast<CPathNode**>(calloc(HashSize(), sizeof(CPathNode*)));
				blocks = firstBlock = NewBlock();
			}
			__forceinline ~CPathNodePool() {
				dbg_out("CALL DESTRUCTOR [ ~CPathNodePool ]");
				Clear();
				free(firstBlock);
				free(cache);
				free(hashTable);
			}
			__forceinline void Clear() {
				Block* b = blocks;
				while (b) {
					Block* temp = b->nextBlock;
					if (b != firstBlock) {
						free(b);
					}
					b = temp;
				}
				blocks = firstBlock;
				if (nAllocated > 0) {
					freeMemSentinel.next = &freeMemSentinel;
					freeMemSentinel.prev = &freeMemSentinel;
					memset(hashTable, 0, sizeof(CPathNode*) * HashSize());
					for (u_4b i = 0; i < CACHBLOCK; ++i) {
						freeMemSentinel.AddBefore(&firstBlock->pathNode[i]);
					}
				}
				nAvailable = CACHBLOCK;
				nAllocated = 0;
				cacheSize = 0;
			}
			__forceinline CPathNode* GetCPathNode(u_4b frame_, u_4b _state, u_4b _costFromStart, u_4b _estToGoal, CPathNode* _parent) {
				u_4b key = _state & HashMask();
				CPathNode* root = hashTable[key];
				while (root) {
					if (root->state == _state) {
						if (root->frame_ == frame_)		// This is the correct state and correct frame_.
							break;
						// Correct state, wrong frame_.
						root->Init(frame_, _state, _costFromStart, _estToGoal, _parent);
						break;
					}
					root = (_state < root->state) ? root->child[0] : root->child[1];
				}
				if (!root) {
					// CACHBLOCK new one
					root = Alloc();
					root->Clear();
					root->Init(frame_, _state, _costFromStart, _estToGoal, _parent);
					AddCPathNode(key, root);
				}
				return root;
			}
			__forceinline Bool PushCache(const SNodeCost* nodes, i_4b nNodes, i_4b* start) {
				*start = -1;
				if (nNodes + cacheSize <= cacheCap) {
					for (i_4b i = 0; i < nNodes; ++i) {
						cache[i + cacheSize] = nodes[i];
					}
					*start = cacheSize;
					cacheSize += nNodes;
					return true;
				}
				return false;
			}
			__forceinline void GetCache(i_4b start, i_4b nNodes, SNodeCost* nodes) {
				memcpy(nodes, &cache[start], sizeof(SNodeCost) * nNodes);
			}
			__forceinline void AllStates(u_4b frame_, CVector< u_4b >* stateVec) {
				for (Block* b = blocks; b; b = b->nextBlock) {
					for (u_4b i = 0; i < CACHBLOCK; ++i) {
						if (b->pathNode[i].frame_ == frame_)
							stateVec->PushBack(b->pathNode[i].state);
					}
				}
			}
			__forceinline u_4b AllocatedNodes() const { return nAllocated; }
			__forceinline u_4b AvailableNodes() const { return nAvailable; }
			__forceinline u_4b HashTableBytes() const { return sizeof(CPathNode*) * static_cast<u_4b>(1 << hashShift); }
			__forceinline u_4b CacheBytes()     const { return sizeof(SNodeCost) * static_cast<u_4b>(cacheCap); }
			__forceinline u_4b BlocksBytes()    const {
				u_4b total = 0;
				for (Block* b = blocks; b; b = b->nextBlock)
					total += sizeof(Block) + sizeof(CPathNode) * (CACHBLOCK - 1);
				return total;
			}
		};
		class CQuadTreeQueue final {
		private:
			CVector<CPathNode*> heap_;
			__forceinline void SiftUp(u_4b put_indx) {
				CPathNode* x = heap_[put_indx];
				CPathNode* y;
				u_4b p;
				while (put_indx > 0) {
					p = (put_indx - 1) >> 2;
					y = heap_[p];
					if (y->totalCost <= x->totalCost) break;
					heap_[put_indx] = y; y->heapIndex = put_indx;
					put_indx = p;
				}
				heap_[put_indx] = x;
				x->heapIndex = put_indx;
			}
			__forceinline void SiftDown(u_4b put_indx) {
				const CPathNode* selected_node = heap_[put_indx];
				const u_4b n = heap_.Size();
				u_4b tree_point, new_best, best, end;
				while (true) {
					tree_point = (put_indx << 2) + 1;
					if (tree_point >= n) {
						break;
					}
					best = tree_point;
					end  = (tree_point + 4);
					if (end > n) {
						end = n;
					}
					for (new_best = tree_point + 1; new_best < end; ++new_best) {
						if (heap_[new_best]->totalCost < heap_[best]->totalCost) {
							best = new_best;
						}
					}
					if (heap_[best]->totalCost >= selected_node->totalCost) {
						break;
					}
					heap_[put_indx] = heap_[best];
					heap_[put_indx]->heapIndex = put_indx;
					put_indx = best;
				}
				heap_[put_indx] = const_cast<CPathNode*>(selected_node);
				const_cast<CPathNode*>(selected_node)->heapIndex = put_indx;
			}
		public:
			__forceinline void Update(CPathNode * n, u_4b oldCost) {
					if (n->totalCost > oldCost) SiftDown(n->heapIndex);
					else SiftUp(n->heapIndex);
			}
			__forceinline void Clear() { heap_.Clear(); }
			__forceinline void Push(CPathNode* n) {
				n->heapIndex = heap_.Size();
				heap_.PushBack(n);
				SiftUp(n->heapIndex);
				n->OpenFlag = 1;
			}
			__forceinline Bool Empty() const { return heap_.Size() == 0; }
			__forceinline CPathNode* Pop() {
				CPathNode* top = heap_[0];
				const u_4b sz = heap_.Size();
				if (sz > 1) {
					CPathNode* last = heap_[sz - 1];
					heap_.Relloc(sz - 1);
					heap_[0] = last;
					last->heapIndex = 0;
					SiftDown(0);
				}
				else {
					heap_.Relloc(0);
				}
				top->OpenFlag = 0;
				top->heapIndex = MAXIMAL;
				return top;
			}
		};
		//======================================================================
#ifdef USE_DUMB_FLAT_MAP_MODE
#define MapTYPE i_1b*
#define FLAT_Func(foo) foo
#define DBLE_Func(foo)
#else
#define MapTYPE i_1b**
#define FLAT_Func(foo)
#define DBLE_Func(foo) foo
#endif
		CPathNodePool pathNodePool_;
		CVector< SStateCost >	statesCostVec_;	// local to Search, but put here to reduce memory allocation
		CVector< SNodeCost  >	nodesCostVec_;	// local to Search, but put here to reduce memory allocation
		u_4b finder_frame_; // incremented with every solve, used to determine if cached data needs to be refreshed
		u_4b total_cost_; // incremented with every solve, used to determine if cached data needs to be refreshed
		u_4b map_size_;
		MapTYPE map_cells_;

#ifdef USE_DUMB_OPTIMIZATE_MAP
		u_4b map_binr_;
		u_4b map_shft_Y_;
		u_4b map_last_;
		u_4b map_mask_X_;
		//======================================================================
		using NegiborsFunc = void(*)(u_4b& s, CVector<SStateCost>& n);
		static void HX_XXX(u_4b& s, CVector<SStateCost>& n) {}
		static void H1_001(u_4b& s, CVector<SStateCost>& n) {
			n.PushBack({ FAST_LINE, s - 1 }); 
		}
		static void H1_010(u_4b& s, CVector<SStateCost>& n) {
			n.PushBack({ FAST_LINE, s + 1 }); 
		}
		static void H1_011(u_4b& s, CVector<SStateCost>& n) {
			n.PushBack({ FAST_LINE, s - 1 });
			n.PushBack({ FAST_LINE, s + 1 });
		}

		static void H2_001(u_4b& s, CVector<SStateCost>& n) {
			n.PushBack({ FAST_DIAG, s - 1 });
		}
		static void H2_010(u_4b& s, CVector<SStateCost>& n) {
			n.PushBack({ FAST_LINE, s });
		}
		static void H2_100(u_4b& s, CVector<SStateCost>& n) {
			n.PushBack({ FAST_DIAG, s + 1 });
		}
		static void H2_101(u_4b& s, CVector<SStateCost>& n) {
			n.PushBack({ FAST_DIAG, s - 1 });
			n.PushBack({ FAST_DIAG, s + 1 });
		}
		static void H2_011(u_4b& s, CVector<SStateCost>& n) {
			n.PushBack({ FAST_DIAG, s - 1 });
			n.PushBack({ FAST_LINE, s });
		}
		static void H2_110(u_4b& s, CVector<SStateCost>& n) {
			n.PushBack({ FAST_LINE, s });
			n.PushBack({ FAST_DIAG, s + 1 });
		}
		static void H2_111(u_4b& s, CVector<SStateCost>& n) {
			n.PushBack({ FAST_DIAG, s - 1 });
			n.PushBack({ FAST_LINE, s });
			n.PushBack({ FAST_DIAG, s + 1 });
		}
		static constexpr NegiborsFunc NegiborsFunc_1[4] = { 
				HX_XXX, H1_001, H1_010, H1_011
			};
		static constexpr NegiborsFunc NegiborsFunc_2[8] = {
				HX_XXX, H2_001, H2_010, H2_011,
				H2_100, H2_101, H2_110, H2_111
			};
		//======================================================================
		__forceinline void GetAdjacentCost(u_4b state, CVector<SStateCost>* neighbors) {
			if (state > map_last_) return;
			CVector<SStateCost>& neig = *neighbors;
			FLAT_Func(i_1b& node_data = map_cells_[state]);
			DBLE_Func(i_1b& node_data = map_cells_[state >> map_shft_Y_][state & map_mask_X_]);
			i_1b node_jmp = (node_data >> 3) & 0b00000011;
			NegiborsFunc_1[node_jmp](state, neig);
			state -= map_size_;
			node_jmp = node_data & 0b00000111;
			NegiborsFunc_2[node_jmp](state, neig);
			state += map_binr_;
			node_jmp = (node_data >> 5) & 0b00000111;
			NegiborsFunc_2[node_jmp](state, neig);
		}
		//======================================================================
		__forceinline u_4b GetEstimateCost(u_4b stateStart, u_4b stateEnd) {
			const u_4b sX = stateStart & map_mask_X_;
			const u_4b sY = stateStart >> map_shft_Y_;
			const u_4b eX = stateEnd & map_mask_X_;
			const u_4b eY = stateEnd >> map_shft_Y_;
			const u_4b dx = sX > eX ? sX - eX : eX - sX;
			const u_4b dy = sY > eY ? sY - eY : eY - sY;
			return (dx + dy) << 7;
		}
		//======================================================================
#else
		__forceinline u_4b GetEstimateCost(u_4b stateStart, u_4b stateEnd) {
			u_4b sX = stateStart % map_size_;
			stateStart = stateStart / map_size_;
			u_4b eX = stateEnd % map_size_;
			stateEnd = stateEnd / map_size_;
			return (
				(stateStart > stateEnd ? stateStart - stateEnd : stateEnd - stateStart) +
				(sX > eX ? sX - eX : eX - sX)  ) << 7;
		}
		__forceinline void GetAdjacentCost(u_4b state, CVector<SStateCost>* neighbors) {
			//For some reason unknown to me, the compiler doesn't optimize this function if it contains a loop, so I unrolled the loop manually!
			u_4b stepY, stepX, X = state % map_size_;
			//----------------------------------------------------------------------
			stepY = (state / map_size_) - 1;
			if (stepY < map_size_) {
				stepX = --X;
				i_1b*& line = map_cells_[stepY];
				u_4b indx = (state - map_size_) - 1;
				SET_DIAG_NEGIBORS(line[stepX]);
				++stepX;
				++indx;
				SET_LINE_NEGIBORS(line[stepX]);
				++stepX;
				++indx;
				SET_DIAG_NEGIBORS(line[stepX]);
			}
			//----------------------------------------------------------------------
			++stepY;
			if (stepY < map_size_) {
				stepX = X;
				i_1b*& line = map_cells_[stepY];
				u_4b indx = state - 1;
				SET_LINE_NEGIBORS(line[stepX]);
				stepX += 2;
				indx += 2;
				SET_LINE_NEGIBORS(line[stepX]);
			}
			//----------------------------------------------------------------------
			++stepY;
			if (stepY < map_size_) {
				stepX = X;
				i_1b*& line = map_cells_[stepY];
				u_4b indx = (state + map_size_) - 1;
				SET_DIAG_NEGIBORS(line[stepX]);
				++stepX;
				++indx;
				SET_LINE_NEGIBORS(line[stepX]);
				++stepX;
				++indx;
				SET_DIAG_NEGIBORS(line[stepX]);
			}
		}
#endif

		//======================================================================
		__forceinline void Achieved(CPathNode* node, u_4b start, u_4b end, CVector<u_4b>* _path) {
			CVector<u_4b>& path = *_path;
			CPathNode* it = node;
			i_4b count = 1;
			path.Clear();
			while (it->parent) {
				++count;
				it = it->parent;
			}
			if (count < 3) {
				path.Relloc(2);
				path[0] = start;
				path[1] = end;
			}
			else {
				path.Relloc(count);
				path[0] = start;
				path[count - 1] = end;
				count -= 2;
				it = node->parent;
				while (it->parent) {
					path[count] = it->state;
					it = it->parent;
					--count;
				}
			}
		}
		__forceinline void Environs(CPathNode* mpather_node, CVector<SNodeCost>* pNodeCost) {
			if (mpather_node->numAdjacent == 0) {
				pNodeCost->Relloc(0);
			}
			else if (mpather_node->cacheIndex < 0) {
				statesCostVec_.Relloc(0);
				GetAdjacentCost(mpather_node->state, &statesCostVec_);
				pNodeCost->Relloc(statesCostVec_.Size());
				mpather_node->numAdjacent = statesCostVec_.Size();
				if (mpather_node->numAdjacent > 0) {
					const u_4b stateCostVecSize = statesCostVec_.Size();
					const SStateCost* stateCostVecPtr = &statesCostVec_[0];
					SNodeCost* pNodeCostPtr = &(*pNodeCost)[0];
					for (u_4b i = 0; i < stateCostVecSize; ++i) {
						u_4b state = stateCostVecPtr[i].state;
						pNodeCostPtr[i].cost = stateCostVecPtr[i].cost;
						pNodeCostPtr[i].node = pathNodePool_.GetCPathNode(finder_frame_, state, MAXIMAL, MAXIMAL, 0);
					}
					i_4b start = 0;
					if (pNodeCost->Size() > 0 && pathNodePool_.PushCache(pNodeCostPtr, pNodeCost->Size(), &start)) {
						mpather_node->cacheIndex = start;
					}
				}
			}
			else {
				pNodeCost->Relloc(mpather_node->numAdjacent);
				SNodeCost* pNodeCostPtr = &(*pNodeCost)[0];
				pathNodePool_.GetCache(mpather_node->cacheIndex, mpather_node->numAdjacent, pNodeCostPtr);
				for (i_4b i = 0; i < mpather_node->numAdjacent; ++i) {
					CPathNode* pNode = pNodeCostPtr[i].node;
					if (pNode->frame_ != finder_frame_) {
						pNode->Init(finder_frame_, pNode->state, MAXIMAL, MAXIMAL, 0);
					}
				}
			}
		}
		__forceinline i_4b Search(u_4b startNode, u_4b endNode, CVector< u_4b >* path) {
			dbg_out("=====================[    BEGIN Search()    ]=====================");
			path->Clear();
			total_cost_ = 0;
			if (startNode == endNode) return AT_THE_END;
			++finder_frame_;
			CQuadTreeQueue open;
			CPathNode *node, *child;
			u_4b oldCost, newCost;
			open.Push(
					pathNodePool_.GetCPathNode(
							finder_frame_, startNode, 0,
							GetEstimateCost(startNode, endNode), 0
						)
				);
			statesCostVec_.Relloc(0);
			nodesCostVec_.Relloc(0);
			while (!open.Empty()) {
				node = open.Pop();
				if (node->state == endNode) {
					total_cost_ = node->costStart;
					Achieved(node, startNode, endNode, path);
					dbg_out("=====================[  IS_SLOVED Search()  ]=====================");
					return IS_SOLVED;
				}
				else {
					node->CloseFlag = 1;
					Environs(node, &nodesCostVec_);
					for (i_4b i = 0; i < node->numAdjacent; ++i) {
						if (nodesCostVec_[i].cost == MAXIMAL) continue;
						child = nodesCostVec_[i].node;
						if (child->OpenFlag || child->CloseFlag) {
							newCost = node->costStart + nodesCostVec_[i].cost;
							if (newCost < child->costStart) {
								oldCost = child->totalCost;
								child->parent = node;
								child->costStart = newCost;
								child->estToGoal = GetEstimateCost(child->state, endNode);
								child->CalcTotalCost();
								if (child->OpenFlag) open.Update(child, oldCost);
							}
						}
						else {
							child->parent = node;
							child->costStart = node->costStart + nodesCostVec_[i].cost;
							child->estToGoal = GetEstimateCost(child->state, endNode), child->CalcTotalCost();
							open.Push(child);
						}
					}
				}
			}
			dbg_out("=====================[ NO_SOLUTION Search() ]=====================");
			return NO_SOLUTION;
		}
	public:
		struct CacheStats final {
			u_4b selfBytes;          // sizeof(CDumbPather)
			u_4b stateCostVecBytes;  // sizeof(StateCost) * statesCostVec_.Size()
			u_4b nodeCostVecBytes;   // sizeof(NodeCost) * nodesCostVec_.Size()
			u_4b poolCacheBytes;     // cache в CPathPool
			u_4b poolBlocksBytes;    // все Block'и
			u_4b poolHashBytes;      // hashTable
			u_4b poolAllocated;      // nAllocated
			u_4b poolAvailable;      // nAvailable
		};
		~CDumbPather() {
			dbg_out("CALL DESTRUCTOR [ ~CDumbPather ]");
		}
		CDumbPather() : map_cells_(nullptr), map_size_(0), pathNodePool_(8), finder_frame_(0),  total_cost_(0) {
			DUMB_OPTIMIZE(
				map_last_ = 0;
				map_binr_ = 0;
				map_mask_X_ = 0;
				map_shft_Y_ = 0;
			);
			dbg_out("CALL CONSTRUCTOR [ CDumbPather ]");
		}
		CDumbPather(CDumbPather&&) = delete;
		CDumbPather(const CDumbPather&) = delete;
		CDumbPather& operator=(CDumbPather&&) = delete;
		CDumbPather& operator=(const CDumbPather&) = delete;

		__forceinline void FindPath(SPoint2u(&path_points)[2], CPathArray* path_segments) {
			dbg_out("=====================[  BEGIN FindPath()  ]=====================");
			SPoint2u& start = path_points[0];
			SPoint2u& end   = path_points[1];
			path_segments->Relloc(0);
			Bool unvalid = (
				(end.y < map_size_ && end.x < map_size_) ?
							DBLE_Func(     map_cells_[end.y][end.x] == TerrainType::BLOKABLE : true             )
							FLAT_Func( map_cells_[(map_size_ * end.y) * end.x] == TerrainType::BLOKABLE : true  )
						) || (
				(start.y < map_size_ && start.x < map_size_) ?
							DBLE_Func(			map_cells_[start.y][start.x] == TerrainType::BLOKABLE : true		)
							FLAT_Func( map_cells_[(map_size_ * start.y) * start.x] == TerrainType::BLOKABLE : true  ) 
						) ;
			if (unvalid) {
				dbg_out("=====================[ NOT_END FindPath() ]=====================");
				return;
			}
			CVector<u_4b> pathNodes;
			u_4b frstNode = map_size_ * static_cast<u_4b>(start.y) + static_cast<u_4b>(start.x),
				lastNode = map_size_ * static_cast<u_4b>(end.y) + static_cast<u_4b>(end.x);
			i_4b result = Search(frstNode, lastNode, &pathNodes);
			if (result == IS_SOLVED) {
				dbg_out("=====================[IS_SOLVED FindPath()]=====================");
				path_segments->Relloc(pathNodes.Size());
				for (u_4b i = 0; i < path_segments->Length(); ++i) {
					(*path_segments)[i] = SPoint2u{
						static_cast<u_2b>(pathNodes[i] % map_size_),
						static_cast<u_2b>(pathNodes[i] / map_size_)
					};
				}
			}
			dbg_out("=====================[   END FindPath()   ]=====================");
		}
		__forceinline void SetByteMap(MapTYPE map_data, u_2b map_size) { //(i_1b** map_data, u_2b map_size) {
			dbg_out("=====================[  BEGIN SetByteMap() ]=====================");
			map_size_ = static_cast<u_4b>(map_size);
			map_cells_ = map_data;
			pathNodePool_.Clear();
			finder_frame_ = 0;
			DUMB_OPTIMIZE(
					map_last_ = (map_size_ * map_size_) - 1;
					map_binr_ = map_size_ << 1;
					map_shft_Y_ = static_cast<u_4b>(std::log2(map_size_));
					map_mask_X_ = map_size_ - 1;
				);
			dbg_out("=====================[   END SetByteMap()  ]=====================");
		}
		__forceinline CacheStats GetCacheStats() const {
			return CacheStats{
				static_cast<u_4b>(sizeof(CDumbPather)),
				static_cast<u_4b>(sizeof(SStateCost) * statesCostVec_.Size()),
				static_cast<u_4b>(sizeof(SNodeCost) * nodesCostVec_.Size()),
				pathNodePool_.CacheBytes(),
				pathNodePool_.BlocksBytes(),
				pathNodePool_.HashTableBytes(),
				pathNodePool_.AllocatedNodes(),
				pathNodePool_.AvailableNodes()
			};
		}
	};
};
#undef SET_LINE_NEGIBORS
#undef SET_DIAG_NEGIBORS
#undef RETURN_HERUINSTIC 
#undef DUMB_UNOPTIMIZE 
#undef DUMB_OPTIMIZE 
#undef CACHBLOCK
#undef MAXIMAL
#endif // PATHFINDER_FOR_DUMBS_INCLUDE
