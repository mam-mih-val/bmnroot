/**
 * \file BmnCluster.h
 * \brief Base class for cluster objects.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \author D. Baranov
 * \date 07.12.2018
 */

#ifndef BMNCLUSTER_H_
#define BMNCLUSTER_H_

#include "TObject.h"
#include <string>
class BmnMatch;

/**
 * \class BmnCluster
 * \brief Base class for cluster objects.
 * \date 07.12.2018
 */
class BmnCluster : public TObject
{
public:
	/**
	 * \brief Constructor.
	 */
	BmnCluster();
	BmnCluster(const std::vector<Int_t>& indices, Int_t address);
	/**
	 * \brief Destructor.
	 */
	virtual ~BmnCluster();

	/**
	 * \brief Add digi to cluster.
	 * \param[in] index Digi index in TClonesArray.
	 */
	void AddDigi(Int_t index) { fDigis.push_back(index); }

	/**
	 * \brief Add array of digi to cluster.
	 * \param[in] indices Array of digi indices in TClonesArray.
	 */
	void AddDigis(const std::vector<Int_t>& indices) { fDigis.insert(fDigis.end(), indices.begin(), indices.end()); }

	/**
	 * \brief Set array of digi to cluster. Overwrites existing array.
	 * \param[in] indices Array of digi indices in TClonesArray.
	 */
	void SetDigis(const std::vector<Int_t>& indices) { fDigis.assign(indices.begin(), indices.end()); }

	/**
	 * \brief Number of digis in cluster.
	 * \return Number of digis in cluster.
	 */
	Int_t GetNofDigis() const { return fDigis.size(); }

	/**
	 * \brief Get digi at position index.
	 * \param[in] index Position of digi in array.
	 * \return Digi index in TClonesArray.
	 */
	Int_t GetDigi(Int_t index) const { return fDigis[index]; }

	/**
	 * \brief Get array of digi indices.
	 * \return Array of digi indices in TClonesArray.
	 */
	const std::vector<Int_t>& GetDigis() const { return fDigis; }

	/**
	 * \brief Remove all digis.
	 */
	void ClearDigis() { fDigis.clear(); }

   /** Accessors **/
   Int_t GetAddress() const { return fAddress; }
   BmnMatch* GetMatch() const { return fMatch; }

   /** Modifiers **/
   void SetAddress(Int_t address) { fAddress = address; }
   void SetMatch(BmnMatch* match);

   /**
    * \brief Return string representation of the object.
    * \return String representation of the object.
    **/
   virtual std::string ToString() const;

private:
   BmnCluster(const BmnCluster&) = delete;
   BmnCluster& operator=(const BmnCluster&) = delete;

	std::vector<Int_t> fDigis; ///< Array of digi indices
	Int_t fAddress; ///< Unique detector ID
	BmnMatch* fMatch; ///< link to Monte-Carlo information

	ClassDef(BmnCluster, 2);
};

#endif /* BMNCLUSTER_H_ */
