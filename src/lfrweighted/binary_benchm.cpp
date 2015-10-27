#include "binary_benchm.h"

/**
 * @brief they_are_mate
 * @param a
 * @param b
 * @param member_list
 * @return
 */
bool they_are_mate(int a, int b, const deque<deque<int> > & member_list)
{

    for(int i=0; i<member_list[a].size(); i++)
    {
        if(binary_search(member_list[b].begin(), member_list[b].end(), member_list[a][i]))
            return true;
    }
    return false;
}

/**
 * @brief deque_int_sum it computes the sum of a deque<int>
 * @param a
 * @return
 */
int deque_int_sum(const deque<int> & a)
{
    int s=0;
    for(int i=0; i<a.size(); i++)
        s+=a[i];

    return s;
}

/**
 * @brief integral it computes the integral of a power law
 * @param a
 * @param b
 * @return
 */
double integral (double a, double b)
{
    if (fabs(a+1.)>1e-10)
        return (1./(a+1.)*pow(b, a+1.));
    else
        return (log(b));
}

/**
 * @brief average_degree it returns the average degree of a power law
 * @param dmax
 * @param dmin
 * @param gamma
 * @return
 */
double average_degree(const double &dmax, const double &dmin, const double &gamma)
{
    return (1./(integral(gamma, dmax)-integral(gamma, dmin)))*(integral(gamma+1, dmax)-integral(gamma+1, dmin));
}


/**
 * @brief solve_dmin bisection method to find the inferior limit, in order to have the expected average degree
 * @param dmax
 * @param dmed
 * @param gamma
 * @return
 */
double solve_dmin(const double& dmax, const double &dmed, const double &gamma)
{
    double dmin_l=1;
    double dmin_r=dmax;
    double average_k1=average_degree(dmin_r, dmin_l, gamma);
    double average_k2=dmin_r;

    if ((average_k1-dmed>0) || (average_k2-dmed<0))
    {
        FILE_LOG(logError) << "ERROR: the average degree is out of range:";
        if (average_k1-dmed>0)
        {
            cerr<<"\nyou should increase the average degree (bigger than "<<average_k1<<")"<<endl;
            cerr<<"(or decrease the maximum degree...)"<<endl;
        }
        if (average_k2-dmed<0)
        {
            cerr<<"\nyou should decrease the average degree (smaller than "<<average_k2<<")"<<endl;
            cerr<<"(or increase the maximum degree...)"<<endl;
        }
        return -1;
    }

    while (fabs(average_k1-dmed)>1e-7)
    {
        double temp=average_degree(dmax, ((dmin_r+dmin_l)/2.), gamma);
        if ((temp-dmed)*(average_k2-dmed)>0)
        {
            average_k2=temp;
            dmin_r=((dmin_r+dmin_l)/2.);
        }
        else
        {

            average_k1=temp;
            dmin_l=((dmin_r+dmin_l)/2.);
        }
    }
    return dmin_l;
}

/**
 * @brief integer_average it computes the correct (i.e. discrete) average of a power law
 * @param n
 * @param min
 * @param tau
 * @return
 */
double integer_average (int n, int min, double tau)
{

    double a=0;

    for (double h=min; h<n+1; h++)
        a+= pow((1./h),tau);

    double pf=0;
    for(double i=min; i<n+1; i++)
        pf+=1/a*pow((1./(i)),tau)*i;

    return pf;

}

/**
 * @brief change_community_size this function changes the community sizes merging the smallest communities
 * @param seq
 * @return
 */
int change_community_size(deque<int> &seq)
{

    if (seq.size()<=2)
        return -1;

    int min1=0;
    int min2=0;

    for (int i=0; i<seq.size(); i++)
        if (seq[i]<=seq[min1])
            min1=i;

    if (min1==0)
        min2=1;

    for (int i=0; i<seq.size(); i++)
        if (seq[i]<=seq[min2] && seq[i]>seq[min1])
            min2=i;

    seq[min1]+=seq[min2];

    int c=seq[0];
    seq[0]=seq[min2];
    seq[min2]=c;
    seq.pop_front();

    return 0;
}

/**
 * @brief build_bipartite_network This function builds a bipartite network with num_seq and member_numbers which are the degree sequences. in member matrix links of the communities are stored. This means member_matrix has num_seq.size() rows and each row has num_seq[i] elements
 * @param member_matrix
 * @param member_numbers
 * @param num_seq
 * @return
 */
int build_bipartite_network(deque<deque<int> >  & member_matrix, const deque<int> & member_numbers, const deque<int> &num_seq)
{
    deque<set<int> > en_in;			// this is the Ein of the subgraph
    deque<set<int> > en_out;		// this is the Eout of the subgraph

    {
        set<int> first;
        for(int i=0; i<member_numbers.size(); i++)
        {
            en_in.push_back(first);
        }
    }

    {
        set<int> first;
        for(int i=0; i<num_seq.size(); i++)
        {
            en_out.push_back(first);
        }
    }

    multimap <int, int> degree_node_out;
    deque<pair<int, int> > degree_node_in;

    for(int i=0; i<num_seq.size(); i++)
        degree_node_out.insert(make_pair(num_seq[i], i));

    for(int i=0; i<member_numbers.size(); i++)
        degree_node_in.push_back(make_pair(member_numbers[i], i));

    sort(degree_node_in.begin(), degree_node_in.end());

    deque<pair<int, int> >::iterator itlast = degree_node_in.end();

    /*
     for (int i=0; i<degree_node_in.size(); i++)
     cout<<degree_node_in[i].first<<" "<<degree_node_in[i].second<<endl;
     */

    while (itlast != degree_node_in.begin())
    {
        itlast--;
        multimap <int, int>::iterator itit= degree_node_out.end();
        deque <multimap<int, int>::iterator> erasenda;

        for (int i=0; i<itlast->first; i++)
        {
            if(itit!=degree_node_out.begin())
            {
                itit--;
                en_in[itlast->second].insert(itit->second);
                en_out[itit->second].insert(itlast->second);

                erasenda.push_back(itit);
            }
            else
                return -1;
        }

        //cout<<"degree node out before"<<endl;
        //prints(degree_node_out);
        for (int i=0; i<erasenda.size(); i++)
        {
            if(erasenda[i]->first>1)
                degree_node_out.insert(make_pair(erasenda[i]->first - 1, erasenda[i]->second));
            degree_node_out.erase(erasenda[i]);
        }
        //cout<<"degree node out after"<<endl;
        //prints(degree_node_out);
    }

    // This is to randomize the subgraph
    deque<int> degree_list;
    for(int kk=0; kk<member_numbers.size(); kk++)
        for(int k2=0; k2<member_numbers[kk]; k2++)
            degree_list.push_back(kk);

    for(int run=0; run<10; run++) for(int node_a=0; node_a<num_seq.size(); node_a++) for(int krm=0; krm<en_out[node_a].size(); krm++)
    {
        int random_mate=degree_list[irand(degree_list.size()-1)];
        if (en_out[node_a].find(random_mate)==en_out[node_a].end())
        {
            deque <int> external_nodes;
            for (set<int>::iterator it_est=en_out[node_a].begin(); it_est!=en_out[node_a].end(); it_est++)
                external_nodes.push_back(*it_est);
            int	old_node=external_nodes[irand(external_nodes.size()-1)];

            deque <int> not_common;
            for (set<int>::iterator it_est=en_in[random_mate].begin(); it_est!=en_in[random_mate].end(); it_est++)
                if (en_in[old_node].find(*it_est)==en_in[old_node].end())
                    not_common.push_back(*it_est);

            if (not_common.empty())
                break;

            int node_h=not_common[irand(not_common.size()-1)];

            en_out[node_a].insert(random_mate);
            en_out[node_a].erase(old_node);

            en_in[old_node].insert(node_h);
            en_in[old_node].erase(node_a);

            en_in[random_mate].insert(node_a);
            en_in[random_mate].erase(node_h);

            en_out[node_h].erase(random_mate);
            en_out[node_h].insert(old_node);
        }
    }

    member_matrix.clear();
    deque <int> first;

    for (int i=0; i<en_out.size(); i++)
    {
        member_matrix.push_back(first);
        for (set<int>::iterator its=en_out[i].begin(); its!=en_out[i].end(); its++)
            member_matrix[i].push_back(*its);
    }
    return 0;
}

/**
 * @brief internal_degree_and_membership
 * @param mixing_parameter
 * @param overlapping_nodes
 * @param max_mem_num
 * @param num_nodes
 * @param member_matrix
 * @param excess
 * @param defect
 * @param degree_seq
 * @param num_seq
 * @param internal_degree_seq
 * @param fixed_range
 * @param nmin
 * @param nmax
 * @param tau2
 * @return
 */
int internal_degree_and_membership (double mixing_parameter, int overlapping_nodes, int max_mem_num, int num_nodes, deque<deque<int> >  & member_matrix,
                                    bool excess, bool defect,  deque<int> & degree_seq, deque<int> &num_seq, deque<int> &internal_degree_seq, bool fixed_range, int nmin, int nmax, double tau2)
{

    if(num_nodes< overlapping_nodes)
    {

        FILE_LOG(logERROR) <<"There are more overlapping nodes than nodes in the whole network! Please, decrease the former ones or increase the latter ones"<<endl;
        return -1;
    }

    member_matrix.clear();
    internal_degree_seq.clear();

    deque<double> cumulative;

    // it assigns the internal degree to each node
    int max_degree_actual=0;		// maximum internal degree

    for (int i=0; i<degree_seq.size(); i++)
    {

        double interno=(1-mixing_parameter)*degree_seq[i];
        int int_interno=int(interno);

        if (ran4()<(interno-int_interno))
            int_interno++;

        if (excess)
        {

            while (   (  double(int_interno)/degree_seq[i] < (1-mixing_parameter) )  &&   (int_interno<degree_seq[i])   )
                int_interno++;

        }

        if (defect)
        {

            while (   (  double(int_interno)/degree_seq[i] > (1-mixing_parameter) )  &&   (int_interno>0)   )
                int_interno--;

        }

        internal_degree_seq.push_back(int_interno);

        if (int_interno>max_degree_actual)
            max_degree_actual=int_interno;

    }

    // it assigns the community size sequence -----------------------------------------------------------------------------

    powerlaw(nmax, nmin, tau2, cumulative);

    if (num_seq.empty())
    {

        int _num_=0;
        if (!fixed_range && (max_degree_actual+1)>nmin)
        {

            _num_=max_degree_actual+1;			// this helps the assignment of the memberships (it assures that at least one module is big enough to host each node)
            num_seq.push_back(max_degree_actual+1);

        }

        while (true)
        {

            int nn=lower_bound(cumulative.begin(), cumulative.end(), ran4())-cumulative.begin()+nmin;

            if (nn+_num_<=num_nodes + overlapping_nodes * (max_mem_num-1) )
            {

                num_seq.push_back(nn);
                _num_+=nn;

            }
            else
                break;

        }

        num_seq[min_element(num_seq.begin(), num_seq.end()) - num_seq.begin()]+=num_nodes + overlapping_nodes * (max_mem_num-1) - _num_;

    }

    //cout<<"num_seq"<<endl;
    //prints(num_seq);

    int ncom=num_seq.size();

    //cout<<"\n----------------------------------------------------------"<<endl;

    /*
     cout<<"community sizes"<<endl;
     for (int i=0; i<num_seq.size(); i++)
     cout<<num_seq[i]<<" ";
     cout<<endl<<endl;
     //*/

    /*
     deque <int> first;
     for (int i=0; i<ncom; i++)
     member_matrix.push_back(first);

     // it puts the overlapping_nodes inside
     cout<<ncom<<endl;
     for (int i=degree_seq.size() - overlapping_nodes; i<degree_seq.size(); i++) {

     cout<<i<<endl;
     set<int> members;
     int hh=0;

     while(members.size()<max_mem_num) {

     int random_module=irand(ncom-1);

     if(member_matrix[random_module].size()!=num_seq[random_module])
     members.insert(random_module);

     hh++;

     if(hh>3*num_nodes) {
     cerr<<"it seems that the overlapping nodes need more communities that those I provided. Please increase the number of communities or decrease the number of overlapping nodes"<<endl;
     return -1;
     }

     }

     for (set<int>::iterator its=members.begin(); its!=members.end(); its++)
     member_matrix[*its].push_back(i);

     }

     // it decides the memberships for the not overlapping nodes

     int moment_module=0;
     for (int i=0; i<num_nodes - overlapping_nodes; i++) {

     while(member_matrix[moment_module].size()==num_seq[moment_module])
     moment_module++;

     member_matrix[moment_module].push_back(i);

     }

     */

    // I have to assign the degree to the nodes

    deque<int> member_numbers;
    for(int i=0; i<overlapping_nodes; i++)
        member_numbers.push_back(max_mem_num);
    for(int i=overlapping_nodes; i<degree_seq.size(); i++)
        member_numbers.push_back(1);

    //prints(member_numbers);
    //prints(num_seq);

    if(build_bipartite_network(member_matrix, member_numbers, num_seq)==-1)
    {

        FILE_LOG(logERROR) << "it seems that the overlapping nodes need more communities that those I provided. Please increase the number of communities or decrease the number of overlapping nodes"<<endl;
        return -1;

    }

    //printm(member_matrix);

    //cout<<"degree_seq"<<endl;
    //prints(degree_seq);

    //cout<<"internal_degree_seq"<<endl;
    //prints(internal_degree_seq);

    deque<int> available;
    for (int i=0; i<num_nodes; i++)
        available.push_back(0);

    for (int i=0; i<member_matrix.size(); i++)
    {
        for (int j=0; j<member_matrix[i].size(); j++)
            available[member_matrix[i][j]]+=member_matrix[i].size()-1;
    }

    deque<int> available_nodes;
    for (int i=0; i<num_nodes; i++)
        available_nodes.push_back(i);

    deque<int> map_nodes;				// in the position i there is the new name of the node i
    for (int i=0; i<num_nodes; i++)
        map_nodes.push_back(0);

    for (int i=degree_seq.size()-1; i>=0; i--)
    {
        int & degree_here=internal_degree_seq[i];
        int try_this = irand(available_nodes.size()-1);
        int kr=0;
        while (internal_degree_seq[i] > available[available_nodes[try_this]])
        {
            kr++;
            try_this = irand(available_nodes.size()-1);
            if(kr==3*num_nodes)
            {
                if(change_community_size(num_seq)==-1)
                {
                    FILE_LOG(logERROR) <<"\n***********************\nERROR: this program needs more than one community to work fine";
                    return -1;
                }
                FILE_LOG(logDEBUG) << "It took too long to decide the memberships; I will try to change the community sizes";
                FILE_LOG(logDEBUG4) <<"new community sizes";
                for (int i=0; i<num_seq.size(); i++)
                    FILE_LOG(logDEBUG4) <<num_seq[i];

                return (internal_degree_and_membership(mixing_parameter, overlapping_nodes, max_mem_num, num_nodes, member_matrix, excess, defect, degree_seq, num_seq, internal_degree_seq, fixed_range, nmin, nmax, tau2));
            }
        }
        map_nodes[available_nodes[try_this]]=i;
        available_nodes[try_this]=available_nodes[available_nodes.size()-1];
        available_nodes.pop_back();
    }

    for (int i=0; i<member_matrix.size(); i++)
    {
        for (int j=0; j<member_matrix[i].size(); j++)
            member_matrix[i][j]=map_nodes[member_matrix[i][j]];
    }

    for (int i=0; i<member_matrix.size(); i++)
        sort(member_matrix[i].begin(), member_matrix[i].end());

    return 0;

}

int compute_internal_degree_per_node(int d, int m, deque<int> & a)
{

    // d is the internal degree
    // m is the number of memebership

    a.clear();
    int d_i= d/m;
    for (int i=0; i<m; i++)
        a.push_back(d_i);

    for(int i=0; i<d%m; i++)
        a[i]++;

    return 0;

}

/*
 int check_link_list(const deque<deque<int> > & link_list, const deque<int> & degree_seq) {

 for (int i=0; i<link_list.size(); i++) {

 int s=0;
 for (int j=0; j<link_list[i].size(); j++)
 s+=link_list[i][j];

 if(s!=degree_seq[i]) {

 int ok;
 cerr<<"wrong link list"<<endl;
 cin>>ok;

 }

 }

 }

 */

/**
 * @brief build_subgraph This function is to build a network with the labels stored in nodes and the degree seq in degrees (correspondence is based on the vectorial index) the only complication is that you don't want the nodes to have neighbors they already have labels will be placed in the end
 * @param E
 * @param nodes
 * @param degrees
 * @return
 */
int build_subgraph(deque<set<int> > & E, const deque<int> & nodes, const deque<int> & degrees)
{
    if(degrees.size()<3)
    {
        FILE_LOG(logWARNING) <<"It seems that some communities should have only 2 nodes! This does not make much sense. Please change some parameters!" << endl;
        return -1;
    }

    deque<set<int> > en; // this is the E of the subgraph

    set<int> first;
    for(int i=0; i<nodes.size(); i++)
        en.push_back(first);

    multimap <int, int> degree_node;

    for(int i=0; i<degrees.size(); i++)
        degree_node.insert(degree_node.end(), make_pair(degrees[i], i));

    int var=0;
    while (degree_node.size() > 0)
    {
        multimap<int, int>::iterator itlast= degree_node.end();
        itlast--;
        multimap <int, int>::iterator itit= itlast;
        deque <multimap<int, int>::iterator> erasenda;

        int inserted=0;
        for (int i=0; i<itlast->first; i++)
        {
            if(itit!=degree_node.begin())
            {
                itit--;
                en[itlast->second].insert(itit->second);
                en[itit->second].insert(itlast->second);
                inserted++;
                erasenda.push_back(itit);
            }
            else
                break;
        }

        for (int i=0; i<erasenda.size(); i++)
        {
            if(erasenda[i]->first>1)
                degree_node.insert(make_pair(erasenda[i]->first - 1, erasenda[i]->second));
            degree_node.erase(erasenda[i]);
        }

        var+= itlast->first - inserted;
        degree_node.erase(itlast);
    }

    deque<int> degree_list;
    for(int kk=0; kk<degrees.size(); kk++)
        for(int k2=0; k2<degrees[kk]; k2++)
            degree_list.push_back(kk);

    // This is to randomize the subgraph
    for(int run=0; run<10; run++) for(int node_a=0; node_a<degrees.size(); node_a++) for(int krm=0; krm<en[node_a].size(); krm++)
    {
        int random_mate=degree_list[irand(degree_list.size()-1)];
        while (random_mate==node_a)
            random_mate=degree_list[irand(degree_list.size()-1)];
        if (en[node_a].insert(random_mate).second)
        {
            deque <int> out_nodes;
            for (set<int>::iterator it_est=en[node_a].begin(); it_est!=en[node_a].end(); it_est++) if ((*it_est)!=random_mate)
                out_nodes.push_back(*it_est);
            int old_node=out_nodes[irand(out_nodes.size()-1)];
            en[node_a].erase(old_node);
            en[random_mate].insert(node_a);
            en[old_node].erase(node_a);
            deque <int> not_common;
            for (set<int>::iterator it_est=en[random_mate].begin(); it_est!=en[random_mate].end(); it_est++)
                if ((old_node!=(*it_est)) && (en[old_node].find(*it_est)==en[old_node].end()))
                    not_common.push_back(*it_est);
            int node_h=not_common[irand(not_common.size()-1)];
            en[random_mate].erase(node_h);
            en[node_h].erase(random_mate);
            en[node_h].insert(old_node);
            en[old_node].insert(node_h);
        }
    }

    // Now I try to insert the new links into the already done network. If some multiple links come out, I try to rewire them
    deque < pair<int, int> > multiple_edge;
    for (int i=0; i<en.size(); i++)
    {
        for(set<int>::iterator its=en[i].begin(); its!=en[i].end(); its++) if(i<*its )
        {
            bool already = !(E[nodes[i]].insert(nodes[*its]).second);		// true is the insertion didn't take place
            if (already)
                multiple_edge.push_back(make_pair(nodes[i], nodes[*its]));
            else
                E[nodes[*its]].insert(nodes[i]);
        }

    }

    FILE_LOG(logDEBUG4)<<"Multiples "<<multiple_edge.size();
    for (int i=0; i<multiple_edge.size(); i++)
    {

        int &a = multiple_edge[i].first;
        int &b = multiple_edge[i].second;
        // Now, I'll try to rewire this multiple link among the nodes stored in nodes.
        int stopper_ml=0;

        while (true)
        {
            stopper_ml++;
            int random_mate=nodes[degree_list[irand(degree_list.size()-1)]];
            while (random_mate==a || random_mate==b)
                random_mate=nodes[degree_list[irand(degree_list.size()-1)]];

            if(E[a].find(random_mate)==E[a].end())
            {
                deque <int> not_common;
                for (set<int>::iterator it_est=E[random_mate].begin(); it_est!=E[random_mate].end(); it_est++)
                    if ((b!=(*it_est)) && (E[b].find(*it_est)==E[b].end()) && (binary_search(nodes.begin(), nodes.end(), *it_est)))
                        not_common.push_back(*it_est);
                if(not_common.size()>0)
                {
                    int node_h=not_common[irand(not_common.size()-1)];
                    E[random_mate].insert(a);
                    E[random_mate].erase(node_h);
                    E[node_h].erase(random_mate);
                    E[node_h].insert(b);
                    E[b].insert(node_h);
                    E[a].insert(random_mate);
                    break;
                }
            }

            if(stopper_ml==2*E.size())
            {
                cout<<"sorry, I need to change the degree distribution a little bit (one less link)"<<endl;
                break;
            }
        }
    }
    return 0;
}

/**
 * @brief build_subgraphs
 * @param E
 * @param member_matrix
 * @param member_list
 * @param link_list
 * @param internal_degree_seq
 * @param degree_seq
 * @param excess
 * @param defect
 * @return
 */
int build_subgraphs(deque<set<int> > & E, const deque<deque<int> > & member_matrix, deque<deque<int> > & member_list,
                    deque<deque<int> > & link_list, const deque<int> & internal_degree_seq, const deque<int> & degree_seq, const bool excess, const bool defect)
{

    E.clear();
    member_list.clear();
    link_list.clear();

    int num_nodes=degree_seq.size();
    //printm(member_matrix);
    {
        deque<int> first;
        for (int i=0; i<num_nodes; i++)
            member_list.push_back(first);
    }

    for (int i=0; i<member_matrix.size(); i++)
        for (int j=0; j<member_matrix[i].size(); j++)
            member_list[member_matrix[i][j]].push_back(i);
    //printm(member_list);
    for (int i=0; i<member_list.size(); i++)
    {
        deque<int> liin;
        for (int j=0; j<member_list[i].size(); j++)
        {
            compute_internal_degree_per_node(internal_degree_seq[i], member_list[i].size(), liin);
            liin.push_back(degree_seq[i] - internal_degree_seq[i]);
        }
        link_list.push_back(liin);
    }

    // Now there is the check for the even node (it means that the internal degree of each group has to be even and we want to assure that, otherwise the degree_seq has to change)
    // This is done to check if the sum of the internal degree is an even number. if not, the program will change it in such a way to assure that.

    for (int i=0; i<member_matrix.size(); i++)
    {
        int internal_cluster=0;
        for (int j=0; j<member_matrix[i].size(); j++)
        {
            int right_index= lower_bound(member_list[member_matrix[i][j]].begin(), member_list[member_matrix[i][j]].end(), i) - member_list[member_matrix[i][j]].begin();

            internal_cluster+=link_list[member_matrix[i][j]][right_index];
        }
        if(internal_cluster % 2 != 0)
        {
            bool default_flag=false;
            if(excess)
                default_flag=true;
            else if(defect)
                default_flag=false;
            else if (ran4()>0.5)
                default_flag=true;

            if(default_flag)
            {

                // if this does not work in a reasonable time the degree sequence will be changed

                for (int j=0; j<member_matrix[i].size(); j++)
                {

                    int random_mate=member_matrix[i][irand(member_matrix[i].size()-1)];

                    int right_index= lower_bound(member_list[random_mate].begin(), member_list[random_mate].end(), i) - member_list[random_mate].begin();

                    if ((link_list[random_mate][right_index]<member_matrix[i].size()-1) && (link_list[random_mate][link_list[random_mate].size()-1] > 0 ))
                    {
                        link_list[random_mate][right_index]++;
                        link_list[random_mate][link_list[random_mate].size()-1]--;
                        break;
                    }
                }
            }
            else
            {
                for (int j=0; j<member_matrix[i].size(); j++)
                {

                    int random_mate=member_matrix[i][irand(member_matrix[i].size()-1)];

                    int right_index= lower_bound(member_list[random_mate].begin(), member_list[random_mate].end(), i) - member_list[random_mate].begin();

                    if (link_list[random_mate][right_index] > 0 )
                    {

                        link_list[random_mate][right_index]--;
                        link_list[random_mate][link_list[random_mate].size()-1]++;

                        break;

                    }

                }

            }

        }

    }

    // This is done to check if the sum of the internal degree is an even number. if not, the program will change it in such a way to assure that.

    set<int> first;
    for(int i=0; i<num_nodes; i++)
        E.push_back(first);

    for (int i=0; i<member_matrix.size(); i++)
    {

        deque<int> internal_degree_i;
        for (int j=0; j<member_matrix[i].size(); j++)
        {

            int right_index= lower_bound(member_list[member_matrix[i][j]].begin(), member_list[member_matrix[i][j]].end(), i) - member_list[member_matrix[i][j]].begin();

            internal_degree_i.push_back(link_list[member_matrix[i][j]][right_index]);

        }

        if(build_subgraph(E, member_matrix[i], internal_degree_i)==-1)
            return -1;

    }

    return 0;

}

int connect_all_the_parts(deque<set<int> > & E, const deque<deque<int> > & member_list, const deque<deque<int> > & link_list)
{

    deque<int> degrees;
    for(int i=0; i<link_list.size(); i++)
        degrees.push_back(link_list[i][link_list[i].size()-1]);

    deque<set<int> > en; // this is the en of the subgraph

    {
        set<int> first;
        for(int i=0; i<member_list.size(); i++)
            en.push_back(first);
    }

    multimap <int, int> degree_node;

    for(int i=0; i<degrees.size(); i++)
        degree_node.insert(degree_node.end(), make_pair(degrees[i], i));

    int var=0;

    while (degree_node.size() > 0)
    {

        multimap<int, int>::iterator itlast= degree_node.end();
        itlast--;

        multimap <int, int>::iterator itit= itlast;
        deque <multimap<int, int>::iterator> erasenda;

        int inserted=0;

        for (int i=0; i<itlast->first; i++)
        {

            if(itit!=degree_node.begin())
            {

                itit--;

                en[itlast->second].insert(itit->second);
                en[itit->second].insert(itlast->second);
                inserted++;

                erasenda.push_back(itit);

            }

            else
                break;

        }

        for (int i=0; i<erasenda.size(); i++)
        {

            if(erasenda[i]->first>1)
                degree_node.insert(make_pair(erasenda[i]->first - 1, erasenda[i]->second));

            degree_node.erase(erasenda[i]);

        }

        var+= itlast->first - inserted;
        degree_node.erase(itlast);

    }

    // this is to randomize the subgraph -------------------------------------------------------------------

    // ----------------------------------------------------------
    deque<int> degree_list;
    for(int kk=0; kk<degrees.size(); kk++)
        for(int k2=0; k2<degrees[kk]; k2++)
            degree_list.push_back(kk);

    for(int run=0; run<10; run++) for(int node_a=0; node_a<degrees.size(); node_a++) for(int krm=0; krm<en[node_a].size(); krm++)
    {

        int random_mate=degree_list[irand(degree_list.size()-1)];
        while (random_mate==node_a)
            random_mate=degree_list[irand(degree_list.size()-1)];

        if (en[node_a].insert(random_mate).second)
        {

            deque <int> out_nodes;
            for (set<int>::iterator it_est=en[node_a].begin(); it_est!=en[node_a].end(); it_est++) if ((*it_est)!=random_mate)
                out_nodes.push_back(*it_est);

            int old_node=out_nodes[irand(out_nodes.size()-1)];

            en[node_a].erase(old_node);
            en[random_mate].insert(node_a);
            en[old_node].erase(node_a);

            deque <int> not_common;
            for (set<int>::iterator it_est=en[random_mate].begin(); it_est!=en[random_mate].end(); it_est++)
                if ((old_node!=(*it_est)) && (en[old_node].find(*it_est)==en[old_node].end()))
                    not_common.push_back(*it_est);

            int node_h=not_common[irand(not_common.size()-1)];

            en[random_mate].erase(node_h);
            en[node_h].erase(random_mate);
            en[node_h].insert(old_node);
            en[old_node].insert(node_h);

        }

    }

    // now there is a rewiring process to avoid "mate nodes" (nodes with al least one membership in common) to link each other

    int var_mate=0;
    for(int i=0; i<degrees.size(); i++) for(set<int>::iterator itss= en[i].begin(); itss!=en[i].end(); itss++) if(they_are_mate(i, *itss, member_list))
    {
        var_mate++;
    }

    //cout<<"var mate = "<<var_mate<<endl;

    int stopper_mate=0;
    int mate_trooper=10;

    while(var_mate>0)
    {

        //cout<<"var mate = "<<var_mate<<endl;

        int best_var_mate=var_mate;

        // ************************************************  rewiring

        for(int a=0; a<degrees.size(); a++) for(set<int>::iterator its= en[a].begin(); its!=en[a].end(); its++) if(they_are_mate(a, *its, member_list))
        {

            int b=*its;
            int stopper_m=0;

            while (true)
            {

                stopper_m++;

                int random_mate =  degree_list[irand(degree_list.size()-1)];
                while (random_mate==a || random_mate==b)
                    random_mate = degree_list[irand(degree_list.size()-1)];

                if(!(they_are_mate(a, random_mate, member_list)) && (en[a].find(random_mate)==en[a].end()))
                {

                    deque <int> not_common;
                    for (set<int>::iterator it_est=en[random_mate].begin(); it_est!=en[random_mate].end(); it_est++)
                        if ((b!=(*it_est)) && (en[b].find(*it_est)==en[b].end()))
                            not_common.push_back(*it_est);

                    if(not_common.size()>0)
                    {

                        int node_h=not_common[irand(not_common.size()-1)];

                        en[random_mate].erase(node_h);
                        en[random_mate].insert(a);

                        en[node_h].erase(random_mate);
                        en[node_h].insert(b);

                        en[b].erase(a);
                        en[b].insert(node_h);

                        en[a].insert(random_mate);
                        en[a].erase(b);

                        if(!they_are_mate(b, node_h, member_list))
                            var_mate-=2;

                        if(they_are_mate(random_mate, node_h, member_list))
                            var_mate-=2;

                        break;

                    }

                }

                if(stopper_m==en[a].size())
                    break;

            }

            break;		// this break is done because if you erased some link you have to stop this loop (en[i] changed)

        }

        // ************************************************  rewiring

        if(var_mate==best_var_mate)
        {

            stopper_mate++;

            if(stopper_mate==mate_trooper)
                break;

        }
        else
            stopper_mate=0;

        //cout<<"var mate = "<<var_mate<<endl;

    }

    //cout<<"var mate = "<<var_mate<<endl;

    for (int i=0; i<en.size(); i++)
    {

        for(set<int>::iterator its=en[i].begin(); its!=en[i].end(); its++) if(i<*its)
        {

            E[i].insert(*its);
            E[*its].insert(i);

        }

    }

    return 0;

}

int internal_kin(deque<set<int> > & E, const deque<deque<int> > & member_list, int i)
{

    int var_mate2=0;
    for(set<int>::iterator itss= E[i].begin(); itss!=E[i].end(); itss++) if(they_are_mate(i, *itss, member_list))
        var_mate2++;

    return var_mate2;

}

int internal_kin_only_one(set<int> & E, const deque<int> & member_matrix_j)  		// return the overlap between E and member_matrix_j
{

    int var_mate2=0;

    for(set<int>::iterator itss= E.begin(); itss!=E.end(); itss++)
    {

        if(binary_search(member_matrix_j.begin(), member_matrix_j.end(), *itss))
            var_mate2++;

    }

    return var_mate2;

}

int erase_links(deque<set<int> > & E, const deque<deque<int> > & member_list, const bool excess, const bool defect, const double mixing_parameter)
{

    int num_nodes= member_list.size();

    int eras_add_times=0;

    if (excess)
    {

        for (int i=0; i<num_nodes; i++)
        {

            while ( (E[i].size()>1) &&  double(internal_kin(E, member_list, i))/E[i].size() < 1 - mixing_parameter)
            {

                //---------------------------------------------------------------------------------

                cout<<"degree sequence changed to respect the option -sup ... "<<++eras_add_times<<endl;

                deque<int> deqar;
                for (set<int>::iterator it_est=E[i].begin(); it_est!=E[i].end(); it_est++)
                    if (!they_are_mate(i, *it_est, member_list))
                        deqar.push_back(*it_est);

                if(deqar.size()==E[i].size())  	// this shouldn't happen...
                {

                    FILE_LOG(logError) << "Something went wrong: there is a node which does not respect the constraints. (option -sup)"<<endl;
                    return -1;
                }

                int random_mate=deqar[irand(deqar.size()-1)];

                E[i].erase(random_mate);
                E[random_mate].erase(i);

            }
        }

    }

    if (defect)
    {

        for (int i=0; i<num_nodes; i++)
            while ( (E[i].size()<E.size()) &&  double(internal_kin(E, member_list, i))/E[i].size() > 1 - mixing_parameter)
            {

                //---------------------------------------------------------------------------------

                cout<<"degree sequence changed to respect the option -inf ... "<<++eras_add_times<<endl;

                int stopper_here=num_nodes;
                int stopper_=0;

                int random_mate=irand(num_nodes-1);
                while ( (    (they_are_mate(i, random_mate, member_list)) || E[i].find(random_mate)!=E[i].end())      &&      (stopper_<stopper_here) )
                {

                    random_mate=irand(num_nodes-1);
                    stopper_++;

                }

                if(stopper_==stopper_here)  	// this shouldn't happen...
                {

                    FILE_LOG(logERROR) << "Something went wrong: there is a node which does not respect the constraints. (option -inf)"<<endl;
                    return -1;
                }

                E[i].insert(random_mate);
                E[random_mate].insert(i);

            }

    }

    //------------------------------------ Erasing links   ------------------------------------------------------

    return 0;

}

