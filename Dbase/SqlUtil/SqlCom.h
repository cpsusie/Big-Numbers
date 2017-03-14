#pragma once

#include "SqlAPI.h"

Packer &operator<<(Packer &p, const SqlApiCreateDb         &crdb      );
Packer &operator>>(Packer &p,       SqlApiCreateDb         &crdb      );
Packer &operator<<(Packer &p, const sqlca                  &ca        );
Packer &operator>>(Packer &p,       sqlca                  &ca        );
Packer &operator<<(Packer &p, const SqlApiCom              &com       );
Packer &operator>>(Packer &p,       SqlApiCom              &com       );
Packer &operator<<(Packer &p, const SqlApiBindProgramId    &programid );
Packer &operator>>(Packer &p,       SqlApiBindProgramId    &programid );
Packer &operator<<(Packer &p, const SqlApiBindStmtHead     &head      );
Packer &operator>>(Packer &p,       SqlApiBindStmtHead     &head      );
Packer &operator<<(Packer &p, const SqlApiBindStmt         &stmt      );
Packer &operator>>(Packer &p,       SqlApiBindStmt         &stmt      );
Packer &operator<<(Packer &p, const StringArray            &strings   );
Packer &operator>>(Packer &p,       StringArray            &strings   );

inline Packer &operator<<(Packer &p, SqlApiCallCode  c) { return p << (int)c;  }
inline Packer &operator>>(Packer &p, SqlApiCallCode &c) { return p >> (int&)c; }

Packer &operator<<(Packer &p, const HostVarDescription     &h  );
Packer &operator>>(Packer &p,       HostVarDescription     &h  );
Packer &operator<<(Packer &p, const HostVarDescriptionList &hl );
Packer &operator>>(Packer &p,       HostVarDescriptionList &hl );

class HostVarList {
  UINT           m_n;
  SqlApiVarList *m_hostvar;
public:
  HostVarList();
  HostVarList(           const HostVarList &src);
  HostVarList &operator=(const HostVarList &src);
  HostVarList(   const HostVarDescriptionList &desc);
  ~HostVarList();
  void describe( const HostVarDescriptionList &desc);
  void clear();
        SqlApiVarList &operator[](int i)       { return m_hostvar[i]; }
  const SqlApiVarList &operator[](int i) const { return m_hostvar[i]; }
  UINT size() const { return m_n;          }
  void dump(FILE *f = stdout);
  friend Packer &operator<<(Packer &p, const HostVarList &h);
  friend Packer &operator>>(Packer &p,       HostVarList &h);
  HostVarDescriptionList getdescription() const;
};

Packer &operator<<(Packer &p, const SqlApiVarList &v);
Packer &operator>>(Packer &p,       SqlApiVarList &v);
