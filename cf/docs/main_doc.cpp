#error "This file is not to be compiled."

//! \defgroup weekly_todo This weeks todo list
//! \brief Contains the weekly todo items.
//! \par Check list for weekly todo
//! \li First scan all the todo items in all the sources.
//! \li Scan all the NOTIMPLEMENTED(); cases.
//! \li Do not delete todos just mark them as done, "\todo Done L Design:...".
//!     After you commit the source in to svn with Done text you may delete the line.
//! \li Add a prefix to todo as to which group it belongs, i.e. Syntax,
//!     Design, Doc, Build. (populate this list as new groups emerge)
//! \li Add a priority to the todo list item, H/M/L, e.g. "H Syntax: ".
//!     Currently assign H for items that is needed immediately for a working prototype,
//!     M is for product related, L is for product final related.

/*! \mainpage Compiler Framework main page
  \sa weekly_todo

  \todo H Coding Style: Statement like macros must be have {} in cases like if (xxx) MACRO();
  \todo H Build: --hash-size=xxx may help improve for linking times.
  \todo H Build: Shall we have a forward declarations header file for all
        classes?
  \todo H Design: Use parameters to signify the required information that is
        related to the function. See, refer StmtPrologEpilogRule::OnCover.
  \todo H Design : Consistently use the same parameter name in the all the
        functions, it makes the debugging much faster; you do not need to
        type different identifiers if there is no GUI support for auto locals.
  \todo M Design: discuss adding operators for irbuilding, i.e. IRExpr*
        operator + (IRExpr*, IRExpr*) : calls the singleton irbuilder and calls
        irbeAdd.
        for addrof return IRExprPtr? define IRExprPtr + IRExpr -> ptradd, etc.
        Also we may prevent the non-copy expression DAG creation errors.
        funcalls (), member subscript [], member access -> can also be
        overloaded for expression/statement creation. make assignment private,
        it is ambiguous, also make new/delete private do not allow "IRExpr*",
        only via irbuilder, GetAddrOf(xxx), or type conversion operator
        (IRExpr*)expr?
  \todo H Design: its okay to copy construct an invalid object.
        ICStmt(INVALID) create a pair<> with default values and it needs copy construction!
  \todo H Design: Discuss IR that has the data structure is responsible from updating it?
  \todo M Design: Remove each rule one by one or in combination and find out
        the assertion failures. This may reveal problems that customers will
        face during their development of CG.
  \todo M Optim: Place an execution point mechanism and be able to attach some
        actions on that execution point, i.e. do optims etc.etc. in that.
  \todo H Design: Discuss messages must flow from parent to child, parent/child
        is determined by aggregation/composition?
  \todo M Coding style: every function with retVal return value looks stupid.
        Perhaps we can change it to "rvXXX", with rv prefix.
  \todo M Build: Add build counting. It is good to distinguish intermediate
        products from each other. Perhaps we should count only when a new
        back-end framework library is generated.
  \todo M Design: convert all c-style array[] in to vectors.
  \todo M CodingStyle: Efficiency: Use ++i instead of i++, use c-tors instead
        of assignment style initialization.
  \todo M CodingStyle: Make class X{friend class base}; put access function to
        X->members in derived:base. Do not populate friends list of X.
  \todo H Design: In objects do not let null effect cases. These may be present in pred/extr/irbuilder
        for convenience (may be even not in irbuilder but in irbuilder helper).
  \todo M Design: Convert all null possible public interface return values in to Invalid
        returning ones.
  \todo H Design: Parameters of some creator/constructors do not match,
        e.g. IRORelocSymbol, irboRelocSymbol. They should match.
  \todo H Design: While creating instructions operands are unnecessarily "new"ed.
  \todo H Design: virtual beg/end of bb statements are causing lots of problems. Change them with
        real statements.
  \todo M Design: Can validity concept be expanded with the use of Predicate?
  \todo M Doc: Convert DebugTrace output to HTML, in a clickable way.
  \todo M Doc: Go over all the comments for the correctness of English.
  \todo M Syntax: Go over all the member and parameter namings.
  \todo Done Design: Make a decision on reference or pointer use in constructors. -> Use ptrs.
  \todo M Design: Is it better to have a reference parameter and expect a non null
        (do the checks before the function?) or have a pointer in the
        function and do the check only in the function?
  \todo H Design: Discuss when VALID DBC should be used.
  \todo M Design: When providing the parameter names in DEBUGTRACE pass the string version of names to DT.
  \todo M Build: Shall we add a build count in to makefile. it helps to distinguish recompiled outputs.
  \todo M Syntax: decide if we need to place initials of class as prefix to the member functions?
  \todo Done M Design: should we have a stmt ctor with InsertContext and get rid of
        parent/prev/next ? -> No, IC in irbuilder.
  \todo M Design: explain pros and cons of CFG auto update by addition of statements, and IR stmt updates by CFG
        modifications.
  \todo M Design: consider adding first & last statements in to basic blocks. May help clean the implementation.
  \todo M Doc: Check that documentation of each use of ENSURE and REQUIRED is up-to-date.
  \todo M Design: discuss a coding style to reduce the required/ensured DBC checks.
  \todo M Design: Discuss about Invalide function and make use of it.
  \todo L Design: Shall we have a new REQUIRED macro for documentation only it will not
        do the actualy check at run-time. Or provide a confidence level??? End run the code according to that
        confidence level?
  \todo L Design: add tests for individual objects copy constructors, assignments. ownership problems with auto_ptrs.
  \todo Done H Design: use make dependencies...
  \todo L Doc: check the use of "\a", "\p", in doxygen docs.
  \todo M Doc: "\note Required/Ensures: " should be replaced by "\post" and "\pre" commands.
  \todo M Doc: use "\retval" command wherever appropriate.
  \todo M Design: add macros to disable DBC macros, may be helpful to check if DBC code is causing a problem.
  \todo M Doc: Add to coding style: doxygen docs are mainly for documenting the code. Therefore
        there will be documentation of member functions at declaration and definition point.
  \todo L Doc: Go over the .vimrc and add in to tips section of wiki.
  \todo L Design: Can we avoid assert checks if all the routines has the ensure/required?
  \todo M Design: Check for each host type usage correctness, i.e. fast and memory efficient?
  \todo M Build: Make doc build depend on source file changes.
  \section ListOfAbbrv List of abbreviations and words to be used in identifier names
  \par You may use full form if the length is not a big issue in the use context.

  \todo L Syntax: check how relational cases are, equal -> Eq|EQ less than...???
  \todo H Marketing: The framework is good for tight release schedules, GCC
        development takes time therefore you may not get an important feature on time.
        Rapid development aspect of the framework is good in these cases.
  \todo M Design: You must be able to disable DBC checks at given srcloc from config file.
  \todo M Design: Implement ICBeforeCFStmt.
  \todo H Design: exprcollector > 1 must have total colection for the correct pre/post order.

  \li address -> addr | Addr
  \li anatrop -> anatrop | Anatrop
  \li and -> and | And
  \li basic block -> bb | BB
  \li beginning -> beg | Beg
  \li boolean -> bool | Bool
  \li calling convention -> callConv | CallConv
  \li column -> col | Col
  \li control flow -> cf | CF
  \li control flow graph -> cfg | CFG
  \li current -> curr | Curr
  \li declaration -> decl | Decl
  \li decrement -> decr | Decr
  \li dereference -> deref | Deref
  \li design by contract -> dbc | DBC
  \li direction -> dir | Dir
  \li element -> elem | Elem
  \li equal -> eq | Eq
  \li evaluate -> eval | Eval
  \li execution -> exe | Exe
  \li expression -> expr | Expr
  \li function -> func | Func
  \li hardware -> hw | HW
  \li identifier -> id | Id
  \li increment -> incr | Incr
  \li insert context -> ic | IC
  \li integer -> int | Int
  \li iteration -> iter | Iter
  \li instruction -> inst | Inst
  \li intermediate representation -> ir | IR
  \li less than -> lt | Lt
  \li less equal -> le | Le
  \li left intentionally empty -> lie | LIE
  \li left hand side -> lhs | LHS
  \li link time -> lt | LT
  \li location -> loc | Loc
  \li negate -> neg | Neg
  \li number -> num | Num
  \li object -> obj | Obj
  \li of -> of | Of
  \li operand -> op | Op
  \li operation -> op | Op
  \li or -> or | Or
  \li parameter -> param | Param
  \li pointer -> ptr | Ptr
  \li predecessor -> pred | Pred
  \li previous -> prev | Prev
  \li program -> prog | Prog
  \li recursive -> rec | Rec
  \li reference -> ref | Ref
  \li return -> ret | Ret
  \li right hand side -> rhs | RHS
  \li statement -> stmt | Stmt
  \li statistics -> stat | Stat
  \li string -> str | Str
  \li successor -> succ | Succ
  \li value -> val | Val
*/

