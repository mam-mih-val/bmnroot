
Int_t tdc72_ChannelConverter(Int_t chInner) {
    const int tdc72vhl_tdcid2tdcnum[9] = {2, 1, 0, 5, 4, 3, 8, 7, 6};
    const int tdc72vhl_tdcch2ch[8] = {7, 6, 5, 4, 3, 2, 1, 0};
    return tdc72vhl_tdcid2tdcnum[chInner / 8] * 8 + tdc72vhl_tdcch2ch[chInner % 8]; //"Rogov to Roumyantsev" conversion.
}