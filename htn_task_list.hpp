#ifndef HTN_TASK_LIST_HPP__
#define HTN_TASK_LIST_HPP__

class HtnTaskList : public std::vector< std::tr1::shared_ptr< HtnTaskDescr > >
{
public:
  HtnTaskList( const std::tr1::shared_ptr< HtnDomain > & p_pDomain,
	       std::string p_sString );

  HtnTaskList( const HtnTaskList & p_Other );

  virtual ~HtnTaskList();

  size_t GetMemSizeMin() const;
  size_t GetMemSizeMax() const;

private:
  std::string m_sName;
  std::tr1::shared_ptr< HtnDomain > m_pDomain;
};

#endif//HTN_TASK_LIST_HPP__
